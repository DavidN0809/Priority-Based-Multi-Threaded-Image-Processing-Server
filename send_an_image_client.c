#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 4096
#define DEFAULT_PORT 2100

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void send_image(int sockfd, const char *filename, int client_number) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) error("ERROR opening file");

    // Send client number to the server first
    int n = write(sockfd, &client_number, sizeof(client_number));
    if (n < 0) error("ERROR writing client number to socket");

   fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    printf("Client %d: Sending image of size %d bytes\n", client_number, file_size);
    rewind(fp);

    // Send image size to the server
    n = write(sockfd, &file_size, sizeof(file_size));
    if (n < 0) error("ERROR writing to socket");

    char buffer[MAX_BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE, fp)) > 0) {
        n = write(sockfd, buffer, bytes_read);
        if (n < 0) error("ERROR writing to socket");
    }
    fclose(fp);
}

void *client_thread(void *arg) {
    int client_number = *((int *)arg);
    int sockfd, portno = DEFAULT_PORT;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    printf("Client %d: Connecting to server...\n", client_number);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    server = gethostbyname("localhost");
    if (server == NULL) error("ERROR, no such host");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) error("ERROR connecting");
    printf("Client %d: Sending image to server...\n", client_number);
    send_image(sockfd, "input.jpg", client_number);
    printf("Client %d: Image sent to server.\n", client_number);

    // Receive the image size back from the server
    int file_size;
    int n = read(sockfd, &file_size, sizeof(file_size));
    if (n < 0) error("ERROR reading actual file size from socket");
    printf("Client %d: Actual file size to receive: %d bytes\n", client_number, file_size);

    char *image_data = malloc(file_size);
    if (image_data == NULL) error("ERROR allocating memory");

// After reading the file size
printf("Client %d: Expected file size to receive: %d bytes\n", client_number, file_size);

int total_bytes_received = 0;
while (total_bytes_received < file_size) {
    n = read(sockfd, image_data + total_bytes_received, file_size - total_bytes_received);
    if (n < 0) error("ERROR reading from socket");
    total_bytes_received += n;
    printf("Client %d: Received %d of %d bytes so far...\n", client_number, total_bytes_received, file_size);
}

printf("Client %d: Finished receiving image data. Total bytes received: %d\n", client_number, total_bytes_received);


printf("Client %d: Finished receiving image data. Total bytes received: %d\n", client_number, total_bytes_received);

    char filename[50];

    printf("Client number: %d is waiting to receive file...\n", client_number);

    sprintf(filename, "./output/received_image-%d.jpg", client_number);
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) error("ERROR opening file");
    fwrite(image_data, file_size, 1, fp);
    fclose(fp);
    
    printf("Client number: %d received file: %s\n", client_number, filename);

    free(image_data);
    close(sockfd);
    return NULL;
}

int main(int argc, char *argv[]) {
    int num_clients = 13;
    if (argc > 1) {
        num_clients = atoi(argv[1]);
        if (num_clients <= 0) num_clients = 13;
    }

    pthread_t threads[num_clients];
    int client_numbers[num_clients];

    for (int i = 0; i < num_clients; ++i) {
        client_numbers[i] = i;
        if (pthread_create(&threads[i], NULL, client_thread, &client_numbers[i]) < 0) {
            error("ERROR creating thread");
        }
    }

    for (int i = 0; i < num_clients; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

