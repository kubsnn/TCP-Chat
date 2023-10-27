# Makefile for a C/C++ project

SRC_DIR = src
LIB_DIR = src/lib
BUILD_DIR = build
BIN_DIR = bin

# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -O3
CXXFLAGS = -Wall -Wunused-variable -fdiagnostics-color=always -std=c++2a -lpthread -I'$(SRC_DIR)/include' -g

# Source files
CPP_SOURCES = $(wildcard $(SRC_DIR)/*.cpp) \
              $(wildcard $(SRC_DIR)/core/*.cpp) \
              $(wildcard $(SRC_DIR)/core/database/*.cpp) \
              $(wildcard $(SRC_DIR)/core/utils/*.cpp)

C_SOURCES = $(wildcard $(SRC_DIR)/include/sqlite/*.c)

# lib files
LIBS = $(wildcard $(LIB_DIR)/*.so.3)

# Object files
CPP_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SOURCES))
C_OBJECTS = $(patsubst $(SRC_DIR)/include/sqlite/%.c, $(BUILD_DIR)/sqlite/%.o, $(C_SOURCES))

# Binary executable name
TARGET = server

$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(BUILD_DIR)/core)
$(shell mkdir -p $(BUILD_DIR)/core/database)
$(shell mkdir -p $(BUILD_DIR)/core/utils)
$(shell mkdir -p $(BUILD_DIR)/sqlite)
$(shell mkdir -p $(BIN_DIR))

# Default target
all: build


# Rule to compile C++ source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile C source files
$(BUILD_DIR)/sqlite/%.o: $(SRC_DIR)/include/sqlite/%.c | $(BUILD_DIR)/sqlite
	$(CC) $(CFLAGS) -c $< -o $@

# Build
build: $(C_OBJECTS) $(CPP_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPP_OBJECTS) $(C_OBJECTS) $(LIBS) -o $(BIN_DIR)/$(TARGET)

# Clean up
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: build
	./$(BIN_DIR)/$(TARGET)

.PHONY: all clean