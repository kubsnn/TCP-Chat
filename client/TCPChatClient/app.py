import eel
import os
import tcp


@eel.expose
def hello_world():
    print("Hello from python")

@eel.expose
def get_greeting(string):
    return f"hello {string}"

@eel.expose
def send_data(ip, port, data):
    tcp.send_data(ip, port, data)

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
