import socket
import ipaddress


def resolve_to_ip(host):
    try:
        # Check if the provided input is already an IP address
        ipaddress.ip_address(host)
        return host  # It's an IP address, no need to resolve
    except ValueError:
        try:
            # It's not an IP address, so resolve it to an IP
            ip = socket.gethostbyname(host)
            return ip
        except socket.gaierror:
            return None  # Unable to resolve the hostname


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
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        resolvedIP = resolve_to_ip(ip)
        s.connect((resolvedIP, port))
        print(s.sendall(data.encode('utf-8')))
        #wait for response
        response = s.recv(1024)
        print(response.decode('utf-8'))
    except socket.error as e:
        print(str(e))
    s.close()


if __name__ == "__main__":
    host = '0.0.0.0'  # Listen on all available network interfaces
    port = 42069  # Choose an available port number

