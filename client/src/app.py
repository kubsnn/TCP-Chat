import eel
import os
import tcp
import threading
from env import get_host_addr, get_port_no



SERVER_ADDR = get_host_addr()
PORT_NO = get_port_no()

client = tcp.Client(None, None)

import sys
import json

@eel.expose
def close_python(*args):
    sys.exit()

@eel.expose
def send_data(data):
    global client
    print(data)
    client.send(data)

@eel.expose
def connect_to_server():
    global client
    client = tcp.Client(tcp.resolve_to_ip(SERVER_ADDR), PORT_NO)
    #if not client.connect():
    #    print("Unable to connect to server!")
    #    eel.show_toast("danger", "Unable to connect to server!", 2000) # type: ignore
    #    return False
    client.connect()
    eel.show_toast("success", "Connected to server!", 2000) # type: ignore

    def listener(response):
        print("Received data: " + json.dumps(response))

    client.set_listener(listener)

    return True

@eel.expose
def listen_to_server():
    return None

@eel.expose
def receive_from_server():
    return None



def initProjectPath():
    script_directory = os.path.dirname(os.path.abspath(__file__))
    # Use the script_directory as the project directory
    project_directory = script_directory
    os.chdir(project_directory)

def startApp():
    global client
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

        if client is not None:
            client.close()


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

