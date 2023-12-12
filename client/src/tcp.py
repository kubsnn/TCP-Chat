import re
import socket
import ipaddress
import struct
import eel
import time
import json
import os
import threading

from cryptography.hazmat.primitives import serialization, hashes
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.backends import default_backend

from typing import Callable, Any



def resolve_to_ip(host) -> str:
    try:
        # Check if the provided input is already an IP address
        ipaddress.ip_address(host)
        return host  # It's an IP address, no need to resolve
    except ValueError:
        try:
            # It's not an IP address, so resolve it to an IP
            return socket.gethostbyname(host)
        except socket.gaierror:
            return ""  # Unable to resolve the hostname

class Client:
    def __init__(self, host: str, port: int):
        """
        Initializes a new instance of the Client class.

        Args:
            host (str): The hostname or IP address of the server.
            port (int): The port number to connect to.
        """
        self.host = resolve_to_ip(host)
        self.port = port
        self.socket = None
        self.private_key, self.public_key = self.__load_or_generate_keys()
        self.server_public_key = None
        self.stop_listener = False

    def connect(self) -> bool:
        """
        Connects to the server.

        Returns:
            bool: True if the connection is successful, False otherwise.
        """
        return self.__initialize_connection(resolve_to_ip(self.host), self.port)

    def set_listener(self, callback: Callable) -> None:
        """
        Sets the listener callback function to handle received data.

        Args:
            callback (callable(dict)): The callback function to handle received data.
        """
        def listener():
            print("Listening...")
            while not self.stop_listener:
                try:
                    data = self.__receive()
                    if data is None: # connection closed
                        break
                    callback(data)
                except:
                    break
            print("Listener thread closed!")

        self.listener_thread = threading.Thread(target=listener)
        #self.listener_thread.daemon = True
        self.listener_thread.start()

    def exit(self):
        try:
            if self.socket is None:
                return

            self.socket.close()
            print("Closing connection...")
            self.stop_listener = True
            self.listener_thread.join(0)
            print("Connection closed!")
        except:
            pass

    def send(self, data: str) -> bool:
        """
        Sends data to the server.

        Args:
            data (str): The data to send.

        Returns:
            bool: True if the data is sent successfully, False otherwise.
        """
        return bool(self.__send(data))

    def close(self) -> None:
        """
        Closes the connection to the server.
        """
        if self.socket is not None:
            self.socket.close()

    def encode(self, string: str) -> str:
        """
        Encodes a string using UTF-8 and escapes special characters.

        Args:
            string (str): The string to encode.

        Returns:
            str: The encoded string.
        """
        return repr(string.encode('utf-8')[1:].decode('unicode_escape'))[1:-1]

# private:
    def __initialize_connection(self, host: str, port: int) -> bool:
        """
        Initializes the connection to the server.

        Args:
            host (str): The hostname or IP address of the server.
            port (int): The port number to connect to.

        Returns:
            bool: True if the connection is successful, False otherwise.
        """
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((host, port))
        except:
            return False

        return self.__exchange_public_keys()

    def __exchange_public_keys(self) -> bool:
        """
        Exchanges public keys with the server.

        Returns:
            bool: True if the public keys are exchanged successfully, False otherwise.
        """
        data = self.__receive_raw()
        if data is None:
            return False

        my_public_key = self.public_key.public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.PKCS1
        )

        self.server_public_key = serialization.load_pem_public_key(
            data["public_key"].encode('utf-8'),
            backend=default_backend()
        )

        my_key = { "action" : "init", "public_key" : my_public_key.decode('utf-8') }
        try:
            self.__send(json.dumps(my_key))
        except:
            print("Server is offline!")
            return False
        return True

    def __send(self, msg: str) -> bool:
        """
        Sends encrypted data to the server.

        Args:
            data (str): The data to send.

        Returns:
            bool: True if the data is sent successfully, False otherwise.
        """

        data = msg.encode('utf-8')
        length = struct.pack('I', len(repr(data)))

        try:
            self.socket.send(length)
            buffer_size = 245
            for i in range(0, len(data), buffer_size):
                count = min(buffer_size, len(data) - i)
                chunk = data[i:i+count]
                encrypted_chunk = self.server_public_key.encrypt(chunk, padding.PKCS1v15())
                self.socket.send(encrypted_chunk)

        except socket.error as e:
            print(str(e))
            return False

        return True

    def __receive(self) -> dict[str, Any]:
        """
        Receives and decrypts data from the server.

        Returns:
            dict: The received data as a dictionary.
        """
        if self.socket is None:
            return None

        length = struct.unpack('I', self.socket.recv(4))[0]
        data = b''
        buffer_size = 256
        for i in range(0, length, buffer_size):
            count = min(buffer_size, length - i)
            try:
                chunk = self.socket.recv(count)
            except:
                return None

            decrypted_chunk = self.private_key.decrypt(chunk, padding.PKCS1v15())
            data += decrypted_chunk

        return json.loads(data.decode('utf-8').rstrip('\0'))

    def __receive_raw(self) -> dict[str, Any]:
        """
        Receives raw data from the server.

        Returns:
            dict: The received data as a dictionary.
        """
        if self.socket is None:
            return None

        try:
            length = struct.unpack('I', self.socket.recv(4))[0]
            data = self.socket.recv(length)
        except:
            print("Server is offline!")
            return None

        return json.loads(data.decode('utf-8').rstrip('\0'))

    def __load_or_generate_keys(self) -> tuple:
        """
        Loads or generates RSA keys.

        Returns:
            tuple: A tuple containing the private key and the public key.
        """
        # Check if the keys already exist
        if os.path.isfile('private_key.pem') and os.path.isfile('public_key.pem'):
            return self.__load_keys()

        # Generate a new RSA key
        key = rsa.generate_private_key(
            public_exponent=65537,
            key_size=2048,
            backend=default_backend()
        )

        self.__write_keys_to_file(key)

        return key, key.public_key()

    def __load_keys(self) -> tuple:
        """
        Loads RSA keys from files.

        Returns:
            tuple: A tuple containing the private key and the public key.
        """
        # Load the keys from files
        with open('private_key.pem', 'rb') as f:
            priv_key = serialization.load_pem_private_key(
                f.read(),
                password=None,
                backend=default_backend()
            )

        with open('public_key.pem', 'rb') as f:
            pub_key = serialization.load_pem_public_key(
                f.read(),
                backend=default_backend()
            )

        return priv_key, pub_key

    def __write_keys_to_file(self, key) -> None:
        """
        Writes RSA keys to files.

        Args:
            key: The RSA key to write.
        """
        # Get the public key in PKCS1 format
        public_key = key.public_key().public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.PKCS1
        )

        # Get the private key in PKCS1 format
        private_key = key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.PKCS8,
            encryption_algorithm=serialization.NoEncryption()
        )

        # Save the keys to files
        with open('private_key.pem', 'wb') as f:
            f.write(private_key)

        with open('public_key.pem', 'wb') as f:
            f.write(public_key)

if __name__ == "__main__":
    host = '127.0.0.1'  # Listen on all available network interfaces
    port = 1100  # Choose an available port number
    client = Client(host, port)
    client.connect()
    client.set_listener(lambda data: print(data["values"][0].encode().decode('unicode-escape')) if "values" in data else print(data)) # print Łukasz

    client.send(json.dumps({"action" : "register", "creds": {"username" : "Łukasz".encode('unicode_escape').decode('utf-8'), "password" : "test"}}))
    client.send(json.dumps({"action" : "login", "creds": {"username" : "Łukasz".encode('unicode_escape').decode('utf-8'), "password" : "test"}}))
    client.send(json.dumps({"action" : "usersOnline"}))


