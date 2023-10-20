# Makefile for a C/C++ project

# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra
CXXFLAGS = -Wall -Wextra -fdiagnostics-color=always -std=c++2a -lpthread -I'include' -g

# Source files
CPP_SOURCES = $(wildcard *.cpp)
C_SOURCES = $(wildcard include/sqlite/*.c)

# Object files
CPP_OBJECTS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))

# Binary executable name
TARGET = server

# Default target
all: build

# Rule to compile C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build
build: $(C_OBJECTS) $(CPP_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPP_OBJECTS) $(C_OBJECTS) -o $(TARGET)

# Clean up
clean:
	rm -f $(TARGET) $(C_OBJECTS) $(CPP_OBJECTS)

.PHONY: all clean
