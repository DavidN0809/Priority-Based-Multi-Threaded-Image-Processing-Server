CC = gcc
CXX = g++
CFLAGS = -O2 -Wall -I .
CXXFLAGS = -std=c++11
LIB = -lpthread -lcurl -lgdal
OPENCV = `pkg-config --libs --cflags opencv4`

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread -L/lib/x86_64-linux-gnu/ -lcurl -lgdal

all: client 

client: echoclient.cpp csapp.o
	$(CXX) $(CXXFLAGS) echoclient.cpp $(OPENCV) csapp.o -o client $(LIB)

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

clean:
	rm -f *.o client *~

