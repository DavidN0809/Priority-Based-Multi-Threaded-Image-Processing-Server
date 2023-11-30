# Compiler and flags
CC = gcc
CFLAGS = -Wall -O2

# Source files
COMMON_SOURCES = csapp.c
CLIENT_SOURCES = client.c $(COMMON_SOURCES)
SERVER_SOURCES = server.c $(COMMON_SOURCES)

# Object files
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)

# Executable names
CLIENT = client
SERVER = server

# Default target
all: $(CLIENT) $(SERVER)

# Client and server programs
$(CLIENT): $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(SERVER): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# To obtain object files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Clean
clean:
	rm -f $(CLIENT) $(SERVER) *.o

# Phony targets
.PHONY: all clean
