import re
import socket
import ipaddress
import struct
import eel
import time
import json
import os
import threading
import env

from cryptography.hazmat.primitives import serialization, hashes
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.backends import default_backend

from typing import Callable, Any

APP_NAME = env.get_app_name()

def resolve_to_ip(host) -> str:
    """
    Resolves a hostname to an IP address.

    :param: host  The hostname to resolve.
    :type: host: str

    :return: The IP address of the hostname.
    :rtype: str
    """
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
    """
    Represents a client that connects to a server using TCP/IP protocol.

    """

    def __init__(self, host: str, port: int):
        """
        Initializes a new instance of the Client class.

        :param: host  The hostname or IP address of the server.
        :type: host: str

        :param: port  The port number to connect to.
        :type: port: int

        """
        self.host = resolve_to_ip(host)
        self.port = port
        self.socket = None
        self.directory = self.__get_app_directory()
        self.private_key, self.public_key = self.__load_or_generate_keys()
        self.server_public_key = None
        self.stop_listener = False

    def connect(self) -> bool:
        """
        Connects to the server.

        :returns: The IP address of the hostname.
        :rtype: bool
        """
        return self.__initialize_connection(resolve_to_ip(self.host), self.port)

    def set_listener(self, callback: Callable) -> None:
        """
        Sets the listener callback function to handle received data.

        :param: callback  The callback function to handle received data.
        :type: callback: Callable
        """

        def listener():
            """
            Listens for incoming data and calls the provided callback function.

            This function continuously listens for incoming data until the `stop_listener` flag is set to True.
            When data is received, it is passed to the provided callback function.

            :return: None
            """
            print("Listening...")
            while not self.stop_listener:
                data = self.__receive()
                if data is None:  # connection closed
                    break
                callback(data)

        self.listener_thread = threading.Thread(target=listener)
        #self.listener_thread.daemon = True
        self.listener_thread.start()

    def exit(self):
        """
        Closes the connection and stops the listener thread.

        :return: None
        """
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

        :param data: The data to send.
        :type data: str
        :return: True if the data is sent successfully, False otherwise.
        :rtype: bool
        """
        return bool(self.__send(data))

    def close(self) -> None:
        """
        Closes the connection to the server.

        :return: None
        """
        if self.socket is not None:
            self.socket.close()

    def encode(self, string: str) -> str:
        """
        Encodes a string using UTF-8 and escapes special characters.

        :param string: The string to encode.
        :type string: str
        :return: The encoded string.
        :rtype: str
        """
        return repr(string.encode('utf-8')[1:].decode('unicode_escape'))[1:-1]

    # private:
    def __initialize_connection(self, host: str, port: int) -> bool:
        """
        Initializes the connection to the server.

        :param host: The hostname or IP address of the server.
        :type host: str

        :param port: The port number to connect to.
        :type host: str

        :return: True if the connection is established successfully, False otherwise.
        :rtype: bool

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

        :return: True if the public keys are exchanged successfully, False otherwise.
        :rtype: bool
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

        :param msg: The data to send.
        :type msg: str
        :return: True if the data is sent successfully, False otherwise.
        :rtype: bool
        """

        data = msg.encode('utf-8')
        length = struct.pack('I', len(repr(data)))

        try:
            if self.socket is None:
                return False
            if self.server_public_key is None:
                return False

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

    def __receive(self) -> dict[str, Any] | None:
        """
        Receives and decrypts data from the server.

        :return: None if the connection is closed, otherwise the received data.
        :rtype: dict[str, Any]  | None
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

    def __receive_raw(self) -> dict[str, Any] | None:
        """
        Receives raw data from the server.

        :return: None if the connection is closed, otherwise the received data.
        :rtype: dict[str, Any] | None
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

    def __get_app_directory(self) -> str:
        """
        Gets the application data directory.

        Returns:
            str: The application data directory.
        """
        # Get the user data directory
        if os.name == 'nt':  # Windows
            user_data_dir = os.getenv('APPDATA') or ""
        else:  # Linux and others
            user_data_dir = os.path.expanduser("~")

        if user_data_dir is None or user_data_dir == "":
            raise Exception("Could not get user data directory")

        app_data_dir = os.path.join(str(user_data_dir), str(APP_NAME))

        # Ensure the directory exists; if not, create it
        os.makedirs(app_data_dir, exist_ok=True)

        return app_data_dir

    def __load_or_generate_keys(self) -> tuple:
        """
        Loads or generates RSA keys.

        :return: A tuple containing the private key and the public key.
        :rtype: tuple
        """
        # Check if the keys already exist
        if os.path.isfile(os.path.join(self.directory, 'private_key.pem')) and os.path.isfile(os.path.join(self.directory, 'public_key.pem')):
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

        :return: A tuple containing the private key and the public key.
        :rtype: tuple
        """
        priv_key_path = os.path.join(self.directory, 'private_key.pem')
        pub_key_path = os.path.join(self.directory, 'public_key.pem')
        # Load the keys from files
        with open(priv_key_path, 'rb') as f:
            priv_key = serialization.load_pem_private_key(
                f.read(),
                password=None,
                backend=default_backend()
            )

        with open(pub_key_path, 'rb') as f:
            pub_key = serialization.load_pem_public_key(
                f.read(),
                backend=default_backend()
            )

        return priv_key, pub_key

    def __write_keys_to_file(self, key) -> None:
        """
        Writes RSA keys to files.

        :param key: The RSA key to write.
        :type key: object
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

        priv_key_path = os.path.join(self.directory, 'private_key.pem')
        pub_key_path = os.path.join(self.directory, 'public_key.pem')

        # Save the keys to files
        with open(priv_key_path, 'wb') as f:
            f.write(private_key)

        with open(pub_key_path, 'wb') as f:
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


