import eel
import os
import tcp
import threading
from env import get_host_addr, get_port_no



SERVER_ADDR = get_host_addr()
PORT_NO = get_port_no()

client_socket = None

import sys

@eel.expose
def close_python(*args):
    sys.exit()

@eel.expose
def send_data(data):
    global client_socket
    try:
        print("Sending data: " + data)
        dataLength = int(len(data))
        print("Sending data of length: " + str(dataLength))
        tcp.send_data(dataLength, client_socket)
        tcp.send_data(data, client_socket)
    except Exception as e:
        print(e)

@eel.expose
def connect_to_server():
    global client_socket
    client_socket = tcp.create_socket(tcp.resolve_to_ip(SERVER_ADDR), PORT_NO)
    if client_socket is None:
        print("Unable to connect to server!")
        eel.show_toast("danger", "Unable to connect to server!", 2000) # type: ignore
        return False

    eel.show_toast("success", "Connected to server!", 2000) # type: ignore
    return True

@eel.expose
def listen_to_server():
    receive_thread = threading.Thread(target=tcp.listen_data, args=(client_socket,))
    receive_thread.start()

@eel.expose
def receive_from_server():
    data = tcp.receive_data(client_socket)
    if data == None:
        eel.show_toast("danger", "No response from server!", 2000) # type: ignore
        return False
    return data



def initProjectPath():
    script_directory = os.path.dirname(os.path.abspath(__file__))
    # Use the script_directory as the project directory
    project_directory = script_directory
    os.chdir(project_directory)

def startApp():
    global client_socket
    initProjectPath()


    # Start the Eel web app
    try:
        #Start the application and pass all initial params below
        print("Initializing web app...")
        eel.init("web")
        print("Starting index.html...")
        eel.start("index.html")

    except (SystemExit, MemoryError, KeyboardInterrupt):

        print("Exiting...")

        if client_socket is not None:
            client_socket.close()


        #Handle errors and the potential hanging python.exe process
        #on windows:
        if os.name == 'nt':
            os.system('taskkill /F /IM python.exe /T')
        #on unix:
        else:
            os.system('pkill -f python')



if __name__ == "__main__":
    print("Starting chat client app...")
    startApp()
