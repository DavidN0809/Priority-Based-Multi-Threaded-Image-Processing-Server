# Priority-Based Pre-Threaded Image Processing Server (Work in Progress)
### TODO
part 4
convert images to greyscale then send
make a pre-threaded version, ie a set number of threads and a master, 
master accepts connection requests puts the connection descriptors in a bounded buffer and the individual worker threads remove requests from the bounded buffer before processing.
make a pool of worker threads

part 5
make the master higher priorty than the worker threads

for demo
add optional flag to add 30 second delay to worker threads
### Compile Opencv Test
make
then run
all inputs are optional,
./send_an_image_client "number of clients" "ip addr" "port"
defaults:port 2100, number of clients: 13, ip addr: localhost

./send_an_image_server "port"
default is 2100


each client sends server the input.jpg, server saves these inside the server folder, will convert to greyscale and send it back, where client then saves it to the output directory. Each image has the clients number appened to it. Ie received_image-1.jpg is client 1s image.


###DEMO
Demo: Set the number of worker threads to the one less than the number of cores on your system. Set the buffer size to twice the number of threads. Insert a delay of 30 seconds for the worker threads. Now send a series of different client requests using multiple clients. Since the master is running at a higher priority and the worker threads are slow to process the requests (due to the delay), the master will preempt the workers whenever there is a request from the client, resulting in the master finding the connection descriptor buffer full. Print appropriate messages from the master and worker threads to demonstrate that the master thread is indeed running at a higher priority.
