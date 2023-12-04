#include "./include/csapp.h"
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define LOOP 5

#define MAX_BUFFER_SIZE 4096
#define DEFAULT_PORT 2100
#define NUM_WORKERS 13
#define QUEUESIZE (NUM_WORKERS * 2) // Set queue size to twice the number of threads

// Global variables and structures
pthread_mutex_t client_count_mutex = PTHREAD_MUTEX_INITIALIZER;
static int client_count = 0; // Global variable to keep track of client numbers
pthread_t worker_threads[NUM_WORKERS];

typedef struct {
    int connfd; // Socket file descriptor for client connections
} task;

typedef struct {
    task buf[QUEUESIZE]; // Buffer to hold tasks
    long head, tail; // Pointers to the head and tail of the queue
    int full, empty; // Flags to indicate if the queue is full or empty
    pthread_mutex_t *mut; // Mutex for synchronizing access to the queue
    pthread_cond_t *notFull, *notEmpty; // Condition variables for queue state
} queue;

// Function prototypes
void queueAdd(queue *q, task in);
void queueDel(queue *q, task *out);
void millisleep(int milliseconds);
void displayQueue(queue *q);
void error(const char *msg);
int generate_client_number();
void *handle_client(void *vargp);
void *consumer(void *q);
queue *queueInit (void);
void queueDelete(queue *q);

void displayQueue(queue *q) {
    int i;
    printf("Queue status: ");
    for (i = 0; i < QUEUESIZE; i++) {
        if (i >= q->head && i < q->tail) {
            printf("F "); // Filled
        } else {
            printf("E "); // Empty
        }
    }
    printf("\n");
}

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
    rio_t rio;
    Rio_readinitb(&rio, connfd);
   int client_number = generate_client_number();
    printf("Server: Processing client %d...\n", client_number);

    millisleep(30000);
    
    printf("Server: Client %d connected.\n", client_number);

        int file_size;
    Rio_readn(connfd, &file_size, sizeof(file_size));
    if (file_size <= 0) {
        fprintf(stderr, "Server: Received empty image or file size error from client %d.\n", client_number);
        Close(connfd);
        return NULL; // Exit the thread
    }

    char *image_data = Malloc(file_size);
    if (image_data == NULL) {
        error("Error: Cannot allocate memory");
    }

    Rio_readn(connfd, image_data, file_size);

char operation[256];
Rio_readlineb(&rio, operation, sizeof(operation));
    
    char inputPath[1024], outputPath[1024];
    sprintf(inputPath, "./server/received_images/received_image_%d.jpg", client_number);
    sprintf(outputPath, "./server/processed_image/processed_image_%d.jpg", client_number);

    FILE *fp = fopen(inputPath, "wb");
    fwrite(image_data, 1, file_size, fp);
    fclose(fp);
    Free(image_data);

    
char cmd[4096]; // Increase the buffer size
snprintf(cmd, sizeof(cmd), "./opencv/convert %s %s %s", inputPath, outputPath, operation);

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

void *consumer(void *q) {
    queue *fifo = (queue *)q;
    task t;

    while (1) {
        printf("Worker thread: Received task, beginning processing.\n");
      //  displayQueue(fifo); // Display queue after receiving a task
        pthread_mutex_lock(fifo->mut);
        while (fifo->empty) {
            pthread_cond_wait(fifo->notEmpty, fifo->mut);
        }
        queueDel(fifo, &t);
      //  displayQueue(fifo); // Display the queue status
        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notFull);
        printf("consumer: received task.\n");

        int connfd = t.connfd;  // Extract the connfd from the task
        handle_client(&connfd);  // Call handle_client with the connfd pointer

        millisleep(100);
    }
    return (NULL);
}


queue *queueInit (void)
{
	queue *q;

	q = (queue *)malloc (sizeof (queue));
	if (q == NULL) return (NULL);

	q->empty = 1;
	q->full = 0;
	q->head = 0;
	q->tail = 0;
	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (q->mut, NULL);
	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notFull, NULL);
	q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notEmpty, NULL);
	
	return (q);
}

void queueDelete (queue *q)
{
	pthread_mutex_destroy (q->mut);
	free (q->mut);	
	pthread_cond_destroy (q->notFull);
	free (q->notFull);
	pthread_cond_destroy (q->notEmpty);
	free (q->notEmpty);
	free (q);
}

void queueAdd (queue *q, task in)
{
        q->buf[q->tail] = in;
	q->tail++;
	if (q->tail == QUEUESIZE)
		q->tail = 0;
	if (q->tail == q->head)
		q->full = 1;
	q->empty = 0;

	return;
}

void queueDel (queue *q, task *out)
{
       *out = q->buf[q->head];

	q->head++;
	if (q->head == QUEUESIZE)
		q->head = 0;
	if (q->head == q->tail)
		q->empty = 1;
	q->full = 0;

	return;
}

void millisleep(int milliseconds)
{
      usleep(milliseconds * 1000);
}

int main() {
    // Initialize the task queue and handle initialization failure
    queue *fifo = queueInit();
    if (fifo == NULL) {
        fprintf(stderr, "Queue initialization failed.\n");
        exit(1);
    }
    // Set the scheduling policy and priority for the main thread
	int policy = SCHED_FIFO;
	struct sched_param param;
	param.sched_priority = sched_get_priority_max(policy);
	if (pthread_setschedparam(pthread_self(), policy, &param) != 0) {
	    perror("Failed to set master thread priority");
	    exit(1);
	}
    // Retrieve and display the current scheduling policy and priority of the main thread
int current_policy;
struct sched_param current_param;
if (pthread_getschedparam(pthread_self(), &current_policy, &current_param) != 0) {
    perror("Failed to get master thread priority");
    exit(1);
}

printf("Master thread's current policy: %d, priority: %d\n", current_policy, current_param.sched_priority);
    // Start the worker threads with a lower priority than the main thread
    printf("Starting %d worker threads.\n", NUM_WORKERS);
	param.sched_priority = sched_get_priority_min(policy) + 1; // Or any value lower than master's priority
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, policy);
	pthread_attr_setschedparam(&attr, &param);

	for (int i = 0; i < NUM_WORKERS; i++) {
	    if (pthread_create(&worker_threads[i], &attr, consumer, fifo) != 0) {
		perror("Failed to create worker thread");
		exit(1);
	    }
	}

pthread_attr_destroy(&attr);

    // Set up the server to listen for incoming client connections
    int listenfd = Open_listenfd(DEFAULT_PORT);
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(struct sockaddr_in);

    // Main loop where the master thread accepts new connections and adds them to the queue, acts as producer
    while (1) {
        printf("Master thread: Accepting new connection...\n");
        int *connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        
        // Lock the queue, wait if it is full, then add the new task
        pthread_mutex_lock(fifo->mut);
        while (fifo->full) {
            pthread_cond_wait(fifo->notFull, fifo->mut);
        }

        task new_task;
        new_task.connfd = *connfdp; // Assign the connection file descriptor to the task
        queueAdd(fifo, new_task); // Add the task to the queue
        displayQueue(fifo); // Display the queue status

        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notEmpty);

        Free(connfdp); // Free the dynamically allocated memory
    }
    // Join worker threads (This part will not be reached in this example)
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(worker_threads[i], NULL);
    }
    // Clean up and delete the task queue, never reached
    queueDelete(fifo);

    return 0;
}

