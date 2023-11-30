# Priority-Based Pre-Threaded Image Processing Server (Work in Progress)

This project is currently under development and aims to create a high-performance image processing server. The server is designed in C/C++ and utilizes the producer-consumer model along with FIFO scheduling to efficiently manage client requests.

## Key Features

- **High-Performance Server**: The core of the project is a server capable of high-performance image processing tasks, designed with efficiency in mind.
  
- **Producer-Consumer Model**: By leveraging this model, the server efficiently handles concurrent client requests without bottlenecks.
  
- **Priority Scheduling**: Implements multi-threading with priority scheduling, ensuring that the most critical tasks are serviced promptly by the main thread, while worker threads are dedicated to processing tasks.

## Progress

This project is a work in progress, and updates will be provided as new milestones are achieved.

### Compile Opencv Test
gcc opencv_test_c.c -o opencv_test_c `pkg-config --cflags --libs opencv`

