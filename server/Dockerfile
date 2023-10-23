# Use a base image that includes a C/C++ development environment
FROM gcc:latest

# Set the working directory inside the container
WORKDIR /app

# Copy the project files into the container
COPY . /app

RUN make

# Expose the port your server will run on (replace with your server's port)
EXPOSE 42069

# Start the server (replace with your server's command)
CMD ["./bin/server", "'0.0.0.0'", "42069"]
