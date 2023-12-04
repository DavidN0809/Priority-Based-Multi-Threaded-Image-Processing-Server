#include "csapp.h"

#define MAX_BUFFER_SIZE 4096
#define DEFAULT_PORT 2100

pthread_mutex_t client_count_mutex = PTHREAD_MUTEX_INITIALIZER;
static int client_count = 0; // Global variable to keep track of client numbers

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int generate_client_number() {
    pthread_mutex_lock(&client_count_mutex);
    int client_number = client_count++;
    pthread_mutex_unlock(&client_count_mutex);
    return client_number;
}

void *handle_client(void *vargp) {
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    
    int client_number = generate_client_number();
    printf("Server: Client %d connected.\n", client_number);

    int file_size;
    Rio_readn(connfd, &file_size, sizeof(file_size));
    printf("Server: Receiving image of size %d bytes from client %d.\n", file_size, client_number);

    char *image_data = Malloc(file_size);
    if (image_data == NULL) {
        error("Error: Cannot allocate memory");
    }

    Rio_readn(connfd, image_data, file_size);

    char inputPath[1024], outputPath[1024];
    sprintf(inputPath, "./server/received_images/received_image_%d.jpg", client_number);
    sprintf(outputPath, "./server/greyscale_images/greyscale_image_%d.jpg", client_number);

    FILE *fp = fopen(inputPath, "wb");
    fwrite(image_data, 1, file_size, fp);
    fclose(fp);
    Free(image_data);

    char cmd[2048];
    sprintf(cmd, "./convert %s %s", inputPath, outputPath);
    system(cmd);

    fp = fopen(outputPath, "rb");
    fseek(fp, 0, SEEK_END);
    int gray_file_size = ftell(fp);
    rewind(fp);

    char *gray_image_data = Malloc(gray_file_size);
    fread(gray_image_data, 1, gray_file_size, fp);
    fclose(fp);

    Rio_writen(connfd, &gray_file_size, sizeof(gray_file_size));
    Rio_writen(connfd, gray_image_data, gray_file_size);

    Free(gray_image_data);
    Close(connfd);
    printf("Server: Client %d disconnected.\n", client_number);

    return NULL;
}

int main() {
    queue *fifo = queueInit();
    // Error handling for fifo == NULL

    // Create worker threads
    pthread_t worker_threads[NUM_WORKERS];
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_create(&worker_threads[i], NULL, consumer, fifo);
        // Set thread scheduling and priority (lower than main thread)
    }

    // Main thread - Producer
    while (1) {
        int connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        task new_task = {connfd};
        // Add new_task to the queue
    }


