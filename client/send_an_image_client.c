#include "csapp.h"
#define MAX_BUFFER_SIZE 4096
#define DEFAULT_PORT 2100
#define DEFAULT_SERVER_IP "localhost"
#define DEFAULT_OPERATION "greyscale"

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

void display_image(const char* imagePath, const char* title) {
    char cmd[1024];
    sprintf(cmd, "display %s %s", imagePath, title);
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "Error executing command: %s\n", cmd);
    }
}


void receive_image(int sockfd, const char* outputImagePath) {
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
}

int main(int argc, char **argv) {
    char *server_ip = DEFAULT_SERVER_IP; // Default server IP
    char *operation = DEFAULT_OPERATION; // Default operation
    char *image_file;
    char *outputImagePath = "output_image.jpg";

    // Argument parsing
    switch (argc) {
        case 2:
            image_file = argv[1];
            break;
        case 3:
            image_file = argv[1];
            operation = argv[2];
            break;
        case 4:
            image_file = argv[1];
            operation = argv[2];
            server_ip = argv[3]; // User-specified server IP
            break;
        default:
            // Display usage instructions if incorrect usage
            fprintf(stderr, "Usage: %s <image file> [operation] [server IP]\n", argv[0]);
            printf("Available operations: greyscale, blur, vflip, hflip\n");
            printf("Default server IP: %s\n", DEFAULT_SERVER_IP);
            exit(1);
    }

    int sockfd = Open_clientfd(server_ip, DEFAULT_PORT);

    printf("Client: Sending image '%s' to server %s...\n", image_file, server_ip);
    send_image(sockfd, image_file);

    // Send the operation type to the server
    Rio_writen(sockfd, operation, strlen(operation));
    Rio_writen(sockfd, "\n", 1); // Send newline character to signify end of operation

    printf("Client: Receiving processed image from server...\n");
    receive_image(sockfd, outputImagePath);

 // Display both images
    display_image(image_file, "Sent Image");
    display_image(outputImagePath, "Received Image");

    Close(sockfd);
    return 0;
}

