import socket

def start_tcp_server(host, port):
    # Create a TCP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Bind the socket to the host and port
    server_socket.bind((host, port))

    # Listen for incoming connections
    server_socket.listen(5)

    print(f"Listening on {host}:{port}")

    return server_socket

def handle_client(client_socket):
    # Receive data from the client
    data = client_socket.recv(1024)
    data = data.decode('utf-8')

    print(f"Received data: {data}")

    # You can process the received data as needed
    # ...

    # Close the client socket
    client_socket.close()

def send_data(ip, port, data):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((ip, port))
    client_socket.send(data.encode('utf-8'))
    client_socket.close()

if __name__ == "__main__":
    host = '0.0.0.0'  # Listen on all available network interfaces
    port = 42069  # Choose an available port number

    send_data('127.0.0.1', 42069, 'Hello from the server!')

    server_socket = start_tcp_server(host, port)

    while True:
        # Accept a connection from a client
        client_socket, client_address = server_socket.accept()


        print(f"Accepted connection from {client_address}")

        # Handle the client in a separate function
        #handle_client(client_socket)

        # Example of sending data to a specific IP and port
