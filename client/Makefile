# Makefile for installing npm and its dependencies, building the app, and running a Python script

# Define the default target (the first one in the file)
all: install build run

# Target for installing npm and its dependencies
install:
	cd TCPChatClient/web-src && npm install
	cd TCPChatClient && pip install -r requirements.txt

# Target for building the app using npm
build:
	cd TCPChatClient/web-src && npm run build

# Target for starting the Python script
run:
	python TCPChatClient

# Target to clean all Node.js files and remove the 'web' directory
clean:
	cd TCPChatClient/web-src && npm clean-install --yes
	rm -rf TCPChatClient/web

# Define phony targets to ensure these targets are always executed
.PHONY: install build run clean
