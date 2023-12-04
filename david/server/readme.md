# Priority-Based Pre-Threaded Image Processing Server (Work in Progress)

## TODO

### Part 4
- Convert images to greyscale then send.
- Make a pre-threaded version, i.e., a set number of threads and a master. 
  - The master accepts connection requests, puts the connection descriptors in a bounded buffer, and the individual worker threads remove requests from the bounded buffer before processing.
  - Make a pool of worker threads.

### Part 5
- Make the master higher priority than the worker threads.
- For demo, add optional flag to add 30-second delay to worker threads.

## Compile OpenCV Test
- Run `make`.
- Then run the compiled program.
- All inputs are optional.
- Client command: `./send_an_image_client "number of clients" "ip addr" "port"`
  - Defaults: port 2100, number of clients: 13, IP addr: localhost.
- Server command: `./send_an_image_server "port"`
  - Default is 2100.

Each client sends the server the `input.jpg`, the server saves these inside the server folder, will convert to greyscale, and send it back, where the client then saves it to the output directory. Each image has the client's number appended to it. For example, `received_image-1.jpg` is client 1's image.

## Demo
- Set the number of worker threads to one less than the number of cores on your system.
- Set the buffer size to twice the number of threads.
- Insert a delay of 30 seconds for the worker threads.
- Now send a series of different client requests using multiple clients.
- Since the master is running at a higher priority and the worker threads are slow to process the requests (due to the delay), the master will preempt the workers whenever there is a request from the client, resulting in the master finding the connection descriptor buffer full.
- Print appropriate messages from the master and worker threads to demonstrate that the master thread is indeed running at a higher priority.

