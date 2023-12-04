#include "csapp.h"
#define MAX_BUFFER_SIZE 4096
#define DEFAULT_PORT 2100
#define DEFAULT_SERVER_IP "10.131.139.240"
#define DEFAULT_OPERATION "greyscale"
#define NUM_OPERATIONS 4
void error(const char *msg) {
    perror(msg);
    exit(1);
}

void send_image(int sockfd, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error: File '%s' does not exist.\n", filename);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    if (file_size == 0) {
        fprintf(stderr, "Error: File '%s' is empty.\n", filename);
        fclose(fp);
        exit(1);
    }
    rewind(fp);
    // Send image size to the server
    Rio_writen(sockfd, &file_size, sizeof(file_size));

    char *buffer = Malloc(MAX_BUFFER_SIZE);
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE, fp)) > 0) {
        Rio_writen(sockfd, buffer, bytes_read);
    }
    fclose(fp);
    Free(buffer);
}

void receive_and_display_image(int sockfd, const char* outputImagePath) {
    // Receive the image size back from the server
    int file_size;
    Rio_readn(sockfd, &file_size, sizeof(file_size));

    char *image_data = Malloc(file_size);
    if (image_data == NULL) {
        error("Error allocating memory for image");
    }

    Rio_readn(sockfd, image_data, file_size);

    // Save the received image to a file
    FILE *fp = fopen(outputImagePath, "wb");
    if (fp == NULL) {
        Free(image_data);
        error("Error opening file to save image");
    }
    fwrite(image_data, 1, file_size, fp);
    fclose(fp);
    Free(image_data);

    // Call the external display program
    char cmd[1024];
    sprintf(cmd, "./display %s", outputImagePath);
    system(cmd);
}

void client_process(int client_num, const char *server_ip, const char *image_file) {
    char *operations[NUM_OPERATIONS] = {"greyscale", "blur", "vflip", "hflip"};
    srand(time(NULL) + client_num); // Seed random number generator
    char *operation = operations[rand() % NUM_OPERATIONS]; // Randomly select an operation

    printf("Client %d: Selected operation: %s\n", client_num, operation);

    int sockfd = Open_clientfd(server_ip, DEFAULT_PORT);
    send_image(sockfd, image_file);

    // Send the operation type to the server
    Rio_writen(sockfd, operation, strlen(operation));
    Rio_writen(sockfd, "\n", 1); // Send newline character to signify end of operation

    char outputImagePath[1024];
    sprintf(outputImagePath, "./output/output_image_client%d.jpg", client_num); // Append client number to file name

    receive_and_display_image(sockfd, outputImagePath);
    Close(sockfd);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <number of clients> <image file>\n", argv[0]);
        exit(1);
    }

    int num_clients = atoi(argv[1]);
    char *image_file = argv[2];

    pid_t pids[num_clients];

    for (int i = 0; i < num_clients; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) { // Child process
            client_process(i, DEFAULT_SERVER_IP, image_file);
            exit(0);
        }
    }

    // Parent process waits for all child processes to finish
    for (int i = 0; i < num_clients; ++i) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}
