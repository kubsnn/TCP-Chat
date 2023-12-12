import re
import eel
import os
from api import API
import threading
from env import get_host_addr, get_port_no



SERVER_ADDR = get_host_addr()
PORT_NO = get_port_no()

def listener(response):
    print("Received data: " + json.dumps(response))

api = API(SERVER_ADDR, PORT_NO, listener)

import sys
import json

@eel.expose
def close_python(*args):
    sys.exit()

@eel.expose
def send_data(data):
    global api
    data = json.loads(data)
    print(data)

    if data["action"] == "register":
        response = api.register(data["creds"]["username"], data["creds"]["password"])
        print(response)
        return response

@eel.expose
def connect_to_server():
    global api
    if not api.connect():
        print("Unable to connect to server!")
        eel.show_toast("danger", "Unable to connect to server!", 2000) # type: ignore
        return False

    eel.show_toast("success", "Connected to server!", 2000) # type: ignore
    return True

@eel.expose
def login_to_server(username, password):
    global api
    response = api.login(username, password)
    print(response)
    if response["result"] == "ok":
        eel.show_toast("success", "Logged in successfully!", 2000) # type: ignore
        return True
    else:
        eel.show_toast("danger", response["message"], 2000) # type: ignore
        return False

@eel.expose
def register_to_server(username, password):
    global api
    response = api.register(username, password)
    print(response)
    if response["result"] == "ok":
        eel.show_toast("success", "Registered successfully!", 2000) # type: ignore
        response = api.login(username, password)
        print(response)
        if response["result"] == "ok":
            return True
        else:
            eel.show_toast("danger", response["message"], 2000) # type: ignore
            return False

    else:
        eel.show_toast("danger", response["message"], 2000) # type: ignore
        return False

@eel.expose
def logout_from_server():
    global api
    response = api.logout()
    print(response)
    if response["result"] == "ok":
        eel.show_toast("success", "Logged out successfully!", 2000) # type: ignore
        return True
    else:
        eel.show_toast("danger", response["message"], 2000) # type: ignore
        return False

@eel.expose
def get_pending_invites():
    global api
    try:
        response = api.invitations()
        print(response)
        if response["result"] == "ok":
            return response["values"]
        else:
            eel.show_toast("danger", response["message"], 2000) # type: ignore
            return []
    except Exception as e:
        print(e)
        return []


@eel.expose
def get_friends():
    global api
    response = api.friends()
    print(response)
    if response["result"] == "ok":
        return response["values"]
    else:
        eel.show_toast("danger", response["message"], 2000) # type: ignore
        return []

@eel.expose
def get_online_users():
    global api
    response = api.getOnlineUsers()
    print(response)
    if response["result"] == "ok":
        return response["values"]
    else:
        eel.show_toast("danger", response["message"], 2000) # type: ignore
        return []

@eel.expose
def get_search_users(search):
    global api
    try:
        response = api.search(search)
        print(response)
        if response["result"] == "ok":
            return response["values"]
        else:
            eel.show_toast("danger", response["message"], 2000) # type: ignore
            return []
    except Exception as e:
        print(e)
        return []



@eel.expose
def add_friend(username):
    global api
    try:
        response = api.invite(username)
        print(response)
        if response["result"] == "ok":
            eel.show_toast("success", "Friend request sent!", 2000) # type: ignore
            return True
        else:
            eel.show_toast("danger", response["message"], 2000) # type: ignore
            return False
    except Exception as e:
        print(e)
        return False

@eel.expose
def accept_invite(username):
    global api
    try:
        response = api.accept(username)
        print(response)
        if response["result"] == "ok":
            eel.show_toast("success", "Friend request accepted!", 2000) # type: ignore
            return True
        else:
            eel.show_toast("danger", response["message"], 2000) # type: ignore
            return False
    except Exception as e:
        print(e)
        return False

@eel.expose
def reject_invite(username):
    global api
    # TODO: Implement this
    # try:
    #     response = api.reject(username)
    #     print(response)
    #     if response["result"] == "ok":
    #         eel.show_toast("success", "Friend request rejected!", 2000) # type: ignore
    #         return True
    #     else:
    #         eel.show_toast("danger", response["message"], 2000) # type: ignore
    #         return False
    # except Exception as e:
    #     print(e)
    #     return False

@eel.expose
def remove_friend(username):
    print("Removing friend: " + username)
    print("Not implemented yet!")



def initProjectPath():
    script_directory = os.path.dirname(os.path.abspath(__file__))
    # Use the script_directory as the project directory
    project_directory = script_directory
    os.chdir(project_directory)


def startApp():
    global api
    initProjectPath()


    # Start the Eel web app
    try:
        #Start the application and pass all initial params below
        print("Initializing web app...")
        eel.init("web")
        print("Starting index.html...")
        eel.start("index.html", port = 0)

    except (SystemExit, MemoryError, KeyboardInterrupt):

        print("Exiting...")

        if api is not None:
            api.close()


        #Handle errors and the potential hanging python.exe process
        #on windows:
        if os.name == 'nt':
            os.system('taskkill /F /IM python.exe /T')
            return 0
        #on unix:
        else:
            os.system('pkill -f python')
            return 0



if __name__ == "__main__":
    print("Starting chat client app...")
    startApp()

