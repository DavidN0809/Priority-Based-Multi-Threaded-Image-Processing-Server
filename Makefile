CC=gcc
CFLAGS=-Wall

all: send_an_image_server send_an_image_client

send_an_image_server: send_an_image_server.o
	$(CC) $(CFLAGS) -o send_an_image_server send_an_image_server.o

send_an_image_client: send_an_image_client.o
	$(CC) $(CFLAGS) -o send_an_image_client send_an_image_client.o

send_an_image_server.o: send_an_image_server.c
	$(CC) $(CFLAGS) -c send_an_image_server.c

send_an_image_client.o: send_an_image_client.c
	$(CC) $(CFLAGS) -c send_an_image_client.c

clean:
	rm -f *.o send_an_image_server send_an_image_client
