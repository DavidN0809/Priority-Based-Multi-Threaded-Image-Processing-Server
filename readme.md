# Priority-Based Pre-Threaded Image Processing Server (Work in Progress)

## Compile OpenCV Test
- Run `make`.
- Then run the compiled program.
- All inputs are optional.
- Client command: `./send_an_image_client "ip addr" "port"`
- Server command: `./send_an_image_server "port"`
server requires, chmod +x ./server, and running as sudo, so $sudo ./server 2100

Each client sends the server the `images.png`, the server saves these inside the server folder, will convert to greyscale, and send it back, where the client then saves it to the output directory. Each image has the client's number appended to it. For example, `received_image-1.jpg` is client 1's image.

### Pop OS
Due to my linux-gnu being installed under lib instead of /usr/ I had to pass this path, remove the -L"path" to use defaults
LIB = -lpthread -L/lib/x86_64-linux-gnu/ -lcurl -lgdal


## Demo
- Set the number of worker threads to one less than the number of cores on your system.
- Set the buffer size to twice the number of threads.
- Insert a delay of 30 seconds for the worker threads.
- Now send a series of different client requests using multiple clients.
- Since the master is running at a higher priority and the worker threads are slow to process the requests (due to the delay), the master will preempt the workers whenever there is a request from the client, resulting in the master finding the connection descriptor buffer full.
- Print appropriate messages from the master and worker threads to demonstrate that the master thread is indeed running at a higher priority.

