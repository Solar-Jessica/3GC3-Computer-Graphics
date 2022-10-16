# set the compilers
CC = gcc
CXX = g++

INCLUDES = usr/include/glm
LIBS =

# set the compiler flags
INCLUDEFLAGS = $(foreach includedir,$(INCLUDES),-I$(includedir))
CFLAGS = -ggdb3 -O0 -Wall $(INCLUDEFLAGS)
CXXFLAGS =  -ggdb3 -O0 -Wall $(INCLUDEFLAGS)
LDFLAGS = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

# add header files here
HDRS =

# add source files here
SRCS = glad.c assignment1.cpp

# generate names of object files
OBJS = $(SRCS:.cpp=.o)

# name of executable
TARGET = assignment1

all: $(TARGET)

$(TARGET): $(OBJS) 
		$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY : clean
clean :
		rm assignment1.o

