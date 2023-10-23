# Makefile for a C/C++ project

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -Wall -O3
CXXFLAGS = -Wall -Wunused-variable -fdiagnostics-color=always -std=c++2a -lpthread -I'$(SRC_DIR)/include' -g

# Source files
CPP_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
HPP_HEADERS = $(wildcard $(SRC_DIR)/*.hpp)
C_SOURCES = $(wildcard $(SRC_DIR)/include/sqlite/*.c)

# Object files
CPP_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SOURCES))
C_OBJECTS = $(patsubst $(SRC_DIR)/include/sqlite/%.c, $(BUILD_DIR)/sqlite/%.o, $(C_SOURCES))

# Binary executable name
TARGET = server

# Default target
all: $(BIN_DIR) build

# Rule to compile C++ source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the 'build' and 'bin' directories
$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

# Rule to create the 'sqlite' directory
$(BUILD_DIR)/sqlite:
	mkdir -p $@

# Rule to compile C source files
$(BUILD_DIR)/sqlite/%.o: $(SRC_DIR)/include/sqlite/%.c | $(BUILD_DIR)/sqlite
	$(CC) $(CFLAGS) -c $< -o $@

# Build
build: $(C_OBJECTS) $(CPP_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPP_OBJECTS) $(C_OBJECTS) -o $(TARGET) & mv $(TARGET) $(BIN_DIR)/

# Clean up
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
