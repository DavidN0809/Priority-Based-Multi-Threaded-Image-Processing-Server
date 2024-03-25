# Priority-Based Multi-Threaded Image Processing Server

This document provides instructions for setting up and using a pre-threaded image processing server. The server supports various image processing operations and can handle multiple client requests concurrently.

## Overview

- **Client**: Send images to the server for processing.
- **Client-Many**: Allows sending multiple images simultaneously from different clients.
- **Server**: Handles image processing requests.
- **Configuration Notes**: Specific settings for Pop!_OS users.

## Client Usage

- **Single Image Processing**:
  - Command: `./send_an_image_client input.jpg blur localhost`
  - `input.jpg`: The input image file.
  - `blur`, `vflip`, `hflip`, `greyscale`: Processing options (default is `greyscale` if none specified).
  - `localhost`: IP address of the server.

## Client-Many Usage

- **Multiple Image Processing**:
  - Command: `./send_an_image_client 12 images.png`
  - `images.png`: The input image file.
  - `12`: Number of clients to simulate.
  - Processing is picked randomly for each client.

## Server Setup

- Command: `sudo ./send_an_image_server`
- **Note**: Requires `sudo` due to thread creation permissions.

## Configuration Notes for Pop!_OS

- **Library Paths**:
  - Default LIB: `-lpthread -L/lib/x86_64-linux-gnu/ -lcurl -lgdal`
  - For Pop!_OS, OpenGL is installed at `-I/usr/local/include/opencv4`. Adjust according to your Linux distribution.

## Demo Setup

1. **Set Worker Threads**: Use one less than the number of cores on your system.
2. **Buffer Size**: Set this to twice the number of threads.
3. **Insert Delay**: Add a 30-second delay for worker threads.
4. **Client Requests**: Send different client requests using multiple clients.
5. **Observe Behavior**:
   - Due to the master thread running at a higher priority, and worker threads being slower (because of the delay), the master will preempt workers when new client requests arrive.
   - This results in the master finding the connection descriptor buffer full.
6. **Logging**: Ensure that messages from both master and worker threads are printed to demonstrate the master thread's higher priority.

## How it works
This system implements a server-client model with image processing capabilities. The server (`server.c`) follows a producer-consumer design and interacts with clients that send images for processing. The server uses OpenCV for image manipulation, handling operations like greyscale conversion, blurring, and flipping images.

### Server (`server.c`)
- **Functionality:** Manages client connections, receives images, processes them using OpenCV functions, and sends back the processed images.
- **Key Components:**
  - **Task Queue:** Utilizes a producer-consumer queue for managing tasks.
  - **Thread Management:** Employs multiple worker threads for concurrent processing.
  - **Image Processing:** Interfaces with OpenCV via `convert`, which performs specified image processing operations.

### Image Processing (`convert`)
- **Functionality:** Applies various image processing operations like greyscale, blur, and flips (vertical and horizontal).
- **Implementation:** Uses OpenCV functions for image manipulation.

### Client (`client.c` and `client-many.c`)
- **Functionality:** 
  - `client.c`: Handles a single client sending an image to the server for processing.
  - `client-many.c`: Manages multiple clients sending images concurrently.
- **Process:** Clients send an image and a desired operation to the server, receive the processed image, and display it using the `display` utility.

### Display Utility (`display.cpp`)
- **Purpose:** Displays images using OpenCV's highgui module.
- **Function:** Takes an image path and a window title as arguments, displaying the image in a window.
