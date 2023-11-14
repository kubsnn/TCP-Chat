import eel
import os
import tcp
from decouple import config

SERVER_ADDR = config('HOST_ADDR')
PORT_NO = int(config('PORT_NO'))


@eel.expose
def send_data(data):
    try:

        dataLength = int(len(data))
        print("Sending data of length: " + str(dataLength))
        tcp.send_data(SERVER_ADDR, PORT_NO, dataLength)
        tcp.send_data(SERVER_ADDR,PORT_NO, data)
    except Exception as e:
        print(e)

def initProjectPath():
    script_directory = os.path.dirname(os.path.abspath(__file__))
    # Use the script_directory as the project directory
    project_directory = script_directory
    os.chdir(project_directory)

def startApp():
    initProjectPath()

    eel.init("web")
    eel.start("index.html")

if __name__ == "__main__":
    startApp()
