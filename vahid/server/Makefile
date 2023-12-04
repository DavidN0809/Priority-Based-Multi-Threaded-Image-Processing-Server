CC = gcc
CXX = g++
CFLAGS = -O2 -Wall -I .
CXXFLAGS = -std=c++11
LIB = -lpthread
OPENCV = `pkg-config --libs --cflags opencv4`

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread -L/lib/x86_64-linux-gnu/ -lcurl -lgdal

all: PriorWorker

PriorWorker: PriorityWorker.cpp csapp.o
	g++ -std=c++11 PriorityWorker.cpp sendreceive.cpp greyscale_CV.cpp `pkg-config --libs --cflags opencv4` csapp.o -o PriorWorker -lpthread

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

clean:
	rm PriorWorker
