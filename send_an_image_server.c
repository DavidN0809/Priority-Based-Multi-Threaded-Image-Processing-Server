#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_BUFFER_SIZE 4096
#define DEFAULT_PORT 2100

int active_connections = 0;
pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void increment_connection_count() {
    pthread_mutex_lock(&connection_mutex);
    active_connections++;
    printf("Active Connections: %d\n", active_connections);
    pthread_mutex_unlock(&connection_mutex);
}

void decrement_connection_count() {
    pthread_mutex_lock(&connection_mutex);
    active_connections--;
    printf("Active Connections: %d\n", active_connections);
    pthread_mutex_unlock(&connection_mutex);
}

void *handle_client(void *newsockfd_ptr) {
    int newsockfd = *((int *)newsockfd_ptr);
    free(newsockfd_ptr);

    increment_connection_count();

    int n, file_size, client_number;
    char filename[50];
    FILE *fp;
    printf("Server: Handling client %d...\n", client_number);
    // Receive client number first
    n = read(newsockfd, &client_number, sizeof(client_number));
    if (n < 0) error("ERROR reading client number from socket");

    // Then receive image size
    n = read(newsockfd, &file_size, sizeof(file_size));
    if (n < 0) error("ERROR reading image size from socket");
    printf("Server: Received image size %d bytes from client %d\n", file_size, client_number);

    char *image_data = malloc(file_size);
    if (image_data == NULL) error("ERROR allocating memory");

    int total_bytes_read = 0;
    while (total_bytes_read < file_size) {
        n = read(newsockfd, image_data + total_bytes_read, file_size - total_bytes_read);
        if (n < 0) error("ERROR reading from socket");
        total_bytes_read += n;
    }
    // Save the received image
    sprintf(filename, "./server/received_images/received_image_%d.jpg", client_number);
    fp = fopen(filename, "wb");
    if (fp == NULL) error("ERROR opening file");
    fwrite(image_data, file_size, 1, fp);
    fclose(fp);
    printf("Server: Received image from client %d. Saving to %s...\n", client_number, filename);
    // Comment out or remove the greyscaling part
   // Convert the image and save the greyscale version
    char inputPath[1024];
    char outputPath[1024];
    sprintf(inputPath, "./server/received_images/received_image_%d.jpg", client_number);
    sprintf(outputPath, "./server/greyscale_images/greyscale_image_%d.jpg", client_number);
    char cmd[2048];
    sprintf(cmd, "./opencv/convert %s %s", inputPath, outputPath);
    system(cmd);

    // Open the greyscale image
    fp = fopen(outputPath, "rb");
    if (fp == NULL) error("ERROR opening greyscale file");

    // Get the size of the greyscale image
    fseek(fp, 0, SEEK_END);
    int greyscale_file_size = ftell(fp);
    rewind(fp);

    // Allocate memory for the greyscale image data
    char *greyscale_image_data = malloc(greyscale_file_size);
    if (greyscale_image_data == NULL) error("ERROR allocating memory for greyscale image");

    // Read the greyscale image into the buffer
    fread(greyscale_image_data, greyscale_file_size, 1, fp);
    fclose(fp);

    // Send the size of the greyscale image back to the client
    printf("Server: Sending greyscale image size %d bytes back to client %d\n", greyscale_file_size, client_number);
    n = write(newsockfd, &greyscale_file_size, sizeof(greyscale_file_size));
    if (n < 0) error("ERROR writing greyscale file size to socket");

    // Send the greyscale image back to the client
    int total_bytes_sent = 0;
    while (total_bytes_sent < greyscale_file_size) {
        n = write(newsockfd, greyscale_image_data + total_bytes_sent, greyscale_file_size - total_bytes_sent);
        if (n < 0) error("ERROR writing to socket");
        total_bytes_sent += n;
    }

    free(greyscale_image_data);
    decrement_connection_count();
    close(newsockfd);
    printf("Server: Greyscale image sent to client %d. Closing connection.\n", client_number);

    return NULL;
}
int main(int argc, char *argv[]) {

int sockfd, newsockfd, portno = DEFAULT_PORT;
struct sockaddr_in serv_addr, cli_addr;
socklen_t clilen;

sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) error("ERROR opening socket");

int opt = 1;
// Set socket to allow multiple connections (this is just good practice)
if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    error("setsockopt");
}


    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);


    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");

        int *newsockfd_ptr = malloc(sizeof(int));
        if (newsockfd_ptr == NULL) error("ERROR allocating memory for socket descriptor");
        *newsockfd_ptr = newsockfd;

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, newsockfd_ptr) < 0) error("ERROR creating thread");
        pthread_detach(thread);
    }

    close(sockfd);
    pthread_mutex_destroy(&connection_mutex);
    return 0;
}

