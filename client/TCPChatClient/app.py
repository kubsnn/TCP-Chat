import eel
import os
import tcp
from decouple import config
import threading


SERVER_ADDR = config('HOST_ADDR')
PORT_NO = int(config('PORT_NO'))

import sys

@eel.expose
def close_python(*args):
    sys.exit()

@eel.expose
def send_data(data):
    try:
        print("Sending data: " + data)
        dataLength = int(len(data))
        print("Sending data of length: " + str(dataLength))
        tcp.send_data(SERVER_ADDR, PORT_NO, dataLength)
        tcp.send_data(SERVER_ADDR,PORT_NO, data)
    except Exception as e:
        print(e)



def serve():
    # Start a TCP server
    server_socket = tcp.start_tcp_server("0.0.0.0", 42069)

    while True:
        # Accept incoming client connections
        client_socket, address = server_socket.accept()
        print(f"Accepted connection from {address[0]}:{address[1]}")

        # Handle the client in a separate thread
        client_thread = threading.Thread(
            target=tcp.handle_client,
            args=(client_socket,)
        )
        client_thread.start()



def initProjectPath():
    script_directory = os.path.dirname(os.path.abspath(__file__))
    # Use the script_directory as the project directory
    project_directory = script_directory
    os.chdir(project_directory)

def startApp():
    initProjectPath()

    # Start the TCP server on different thread
    server_thread = threading.Thread(target=serve)
    server_thread.start()

    # Start the Eel web app
    try:
        #Start the application and pass all initial params below
        eel.init("web")
        eel.start("index.html")
    except (SystemExit, MemoryError, KeyboardInterrupt):
        #Handle errors and the potential hanging python.exe process
        os.system('taskkill /F /IM python.exe /T')




if __name__ == "__main__":
    startApp()
