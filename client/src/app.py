import dis
from email import message
import re
import eel
import os
from api import API
import threading
from env import get_host_addr, get_port_no
import messagesDB

SERVER_ADDR = get_host_addr()
PORT_NO = get_port_no()

def listener(response):
    print("Received data: " + json.dumps(response))
    #Received data: {"from": "Aszyk", "to": "Mateusz", "message": "Hejka misiu", "type": "message", "action": "received"}

    #Add this message to DB
    if response["action"] == "received":
        messages = connect_to_db(response["to"])
        if messages is not None:
            messages.insert_message(response["from"], response["message"], False)
            eel.update_messages(messages.get_messages()) # type: ignore
        else:
            print("No messages DB connected!")
        disconnect_from_db(messages)

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
        messages = connect_to_db(username)
        if messages is not None:
            eel.update_messages(messages.get_messages()) # type: ignore
        else:
            print("No messages DB connected!")
        disconnect_from_db(messages)
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
        messages = connect_to_db(username)
        if messages is not None:
            eel.update_messages(messages.get_messages()) # type: ignore
        else:
            print("No messages DB connected!")
        disconnect_from_db(messages)

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
def get_all_messages(username):
    messages = connect_to_db(username)
    if messages is not None:
        print("Getting all messages...")
        all_messages = messages.get_messages()
        disconnect_from_db(messages)
        return all_messages
    else:
        print("No messages DB connected!")
        disconnect_from_db(messages)
        return []


def connect_to_db(username):
    try:
        db = messagesDB.MessagesDB(username)
        return db
    except Exception as e:
        print(e)
        eel.show_toast("danger", "Unable to connect to database!", 2000) # type: ignore
        return None

def disconnect_from_db(db):
    if db is not None:
        db.close()
        db = None

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
    try:
        response = api.remove(username)
        print(response)
        if response["result"] == "ok":
            eel.show_toast("success", "Friend removed!", 2000) # type: ignore
            return True
        else:
            eel.show_toast("danger", response["message"], 2000) # type: ignore
            return False
    except Exception as e:
        print(e)
        return False

@eel.expose
def send_message(sender, recipient, message):
    global api
    try:
        response = api.send(recipient, message)
        print(response)
        if response["result"] == "ok":
            #Add message to DB
            messages = connect_to_db(sender)
            if messages is not None:
                messages.insert_message(recipient, message, True)
                eel.update_messages(messages.get_messages()) # type: ignore
            else:
                print("No messages DB connected!")

            disconnect_from_db(messages)
            return True
        else:
            eel.show_toast("danger", response["message"], 2000) # type: ignore
            return False
    except Exception as e:
        print(e)
        return False


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

