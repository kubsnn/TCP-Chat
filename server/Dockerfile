# Use a base image that includes a C/C++ development environment
FROM gcc:latest

# Install dependencies (replace with your own)
RUN apt-get update && apt-get install -y libssl-dev
RUN apt-get install cmake -y
RUN apt-get install make -y
RUN apt-get install sqlite3 -y
RUN apt-get install libsqlite3-dev -y

# Set the working directory inside the container
WORKDIR /app

# Copy the project files into the container
COPY . /app

RUN cmake .

RUN make

# Expose the port your server will run on (replace with your server's port)
EXPOSE 42069

# Start the server (replace with your server's command)
CMD ["./bin/app", "0.0.0.0", "42069"]