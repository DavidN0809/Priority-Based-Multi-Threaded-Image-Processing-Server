CC=g++
CFLAGS=-I/usr/local/include/opencv4 -Wall
LDFLAGS=-L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs

all: convert display

convert: convert.o
	$(CC) -o convert convert.o $(LDFLAGS)

display: display.o
	$(CC) -o display display.o $(LDFLAGS)

convert.o: convert.cpp
	$(CC) $(CFLAGS) -c convert.cpp

display.o: display.cpp
	$(CC) $(CFLAGS) -c display.cpp

clean:
	rm -f *.o convert display

