# Client Documentation

## Binaries

### Prerequisites

- [Python](https://www.python.org/) (v3.11 or higher)
- [Chrome/Chromium](https://www.chromium.org/chromium-projects/)

Go to the [releases](https://github.com/kubsnn/TCP-Chat.git/releases) page and download the latest version of the client for your OS.
Run the executable file

## Development

### Prerequisites

- [Node.js](https://nodejs.org/en/) (v12.16.1 or higher)
- [Python](https://www.python.org/) (v3.11 or higher)
- [Taskfile](https://taskfile.dev/#/) (v3.0.0)
- [Chrome/Chromium](https://www.chromium.org/chromium-projects/)

### Installation

1. Clone the repo

```sh
git clone https://github.com/kubsnn/TCP-Chat.git
```

2. Go to the client directory

```sh
cd client
```

3. Run Taskfile

```sh
task install
task build
task run
```

This step will install all the dependencies, build the project and start the development app.

### Backend

Client Backend is written in python and is in `/client/src` folder

### Frontend

Client Frontend is written in Vue and is in `/client/src/web-src/` folder




