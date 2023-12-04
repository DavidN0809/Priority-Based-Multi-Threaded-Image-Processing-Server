CC = gcc
CXX = g++
CFLAGS = -Wall -pthread
CXXFLAGS = -Wall -I/usr/local/include/opencv4
LDFLAGS = -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs

# Define source files and their corresponding object files
SERVER_SRCS = send_an_image_server.c csapp.c
CONVERT_SRCS = opencv/convert.cpp
COMMON_SRCS = csapp.c

OBJ_DIR = obj
SERVER_OBJS = $(addprefix $(OBJ_DIR)/,$(SERVER_SRCS:.c=.o))
CONVERT_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(CONVERT_SRCS:.cpp=.o)))
COMMON_OBJS = $(addprefix $(OBJ_DIR)/,$(COMMON_SRCS:.c=.o))

all: send_an_image_server opencv/convert

send_an_image_server: $(SERVER_OBJS) $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

opencv/convert: $(CONVERT_OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: opencv/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o send_an_image_server opencv/convert

