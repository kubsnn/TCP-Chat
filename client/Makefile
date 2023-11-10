# Makefile for installing npm and its dependencies, building the app, and running a Python script

# Define the default target (the first one in the file)
all: install build run

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  install - Install npm and its dependencies"
	@echo "  build   - Build the app using npm"
	@echo "  run     - Run the Python script"
	@echo "  clean   - Clean all Node.js files and remove the 'web' directory"
	@echo "  binaries- Build the binaries using pyinstaller"

# Target for installing npm and its dependencies
install:
	cd TCPChatClient/web-src && npm install
	cd TCPChatClient && pip install -r requirements.txt

# Target for building the app using npm
build:
	rm -rf TCPChatClient/web/assets
	cd TCPChatClient/web-src && npm run build

# Target for starting the Python script
run:
	python TCPChatClient

# Target for building the binaries using pyinstaller
binaries: install build
	pip install pyinstaller
	pyinstaller -n TCPChatClient --add-data ".\TCPChatClient\web\:web" --add-data ".\TCPChatClient\.env:." -p .\TCPChatClient\ .\TCPChatClient\app.py --onefile

# Target to clean all Node.js files and remove the 'web' directory
clean:
	cd TCPChatClient/web-src && npm clean-install --yes
	rm -rf TCPChatClient/web

# Define phony targets to ensure these targets are always executed
.PHONY: install build run clean help
