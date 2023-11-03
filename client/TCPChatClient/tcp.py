import socket
import ipaddress
import struct
import eel

messageStorage = []


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

def create_socket(server_ip, server_port):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((server_ip, server_port))
        return client_socket
    except socket.error as e:
        print(str(e))

# Function to listen for messages from the server
def receive_data(client_socket):
    print("Listening for messages from the server...")
    while True:
        try:
            message = client_socket.recv(1024).decode('utf-8')
            if not message:
                break
            print("Received:", message)
            eel.show_toast("success", "Received: " + message, 5000)
            #add To messageStorage
            messageStorage.append(message)

        except ConnectionResetError:
            break


def send_data(data, client_socket):
    try:
        #do not encode if data is int
        if isinstance(data, int):
            # Convert the integer to 4 bytes
            #data.to_bytes(4, byteorder='big', signed=True)
            data = struct.pack('I', data)
            print("Size Send Response: ", client_socket.send(data))
            return
        #encode data as utf-8
        data = data.encode('utf-8')
        print("Message Send Response: ", client_socket.send(data))
    except socket.error as e:
        print(str(e))



if __name__ == "__main__":
    host = '0.0.0.0'  # Listen on all available network interfaces
    port = 42069  # Choose an available port number

