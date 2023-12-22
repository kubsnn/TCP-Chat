import eel
import os
from api import API
from env import get_host_addr, get_port_no
import messagesDB

SERVER_ADDR = get_host_addr()
PORT_NO = get_port_no()

def listener(response):
    """
    This function is called when a message is received from the server

    :param response: The message received from the server
    :type response: dict

    :return: None
    """
    print("Received data: " + json.dumps(response))
    #Received data: {"from": "Aszyk", "to": "Mateusz", "message": "Hejka misiu", "type": "message", "action": "received"}

    #Add this message to DB
    if response["action"] == "received":
        messages = connect_to_db(response["to"])
        if messages is not None:
            messages.insert_message(response["from"], response["message"], False)
            eel.message_from(response["from"]) # type: ignore
            #eel.update_messages(messages.get_messages()) # type: ignore
        else:
            print("No messages DB connected!")
        disconnect_from_db(messages)

api = API(SERVER_ADDR, PORT_NO, listener)

import sys
import json

@eel.expose
def close_python(*args):
    """
    The function `close_python` exits the Python interpreter.
    """
    sys.exit()

@eel.expose
def send_data(data):
    """
    The function `send_data` takes in data, parses it as JSON, and if the action is "register", it calls
    the `register` method of the `api` object with the provided username and password and returns the
    response.

    :param data: The `data` parameter is a string that contains JSON data.
    :return: The response from the API is being returned.
    """
    global api
    data = json.loads(data)
    print(data)

    if data["action"] == "register":
        response = api.register(data["creds"]["username"], data["creds"]["password"])
        print(response)
        return response

@eel.expose
def connect_to_server():
    """
    The function `connect_to_server` attempts to connect to a server and displays a success or error
    message using a toast notification.
    :return: The function `connect_to_server()` returns a boolean value. It returns `True` if the
    connection to the server is successful, and `False` if the connection fails.
    """
    global api
    if not api.connect():
        print("Unable to connect to server!")
        eel.show_toast("danger", "Unable to connect to server!", 2000) # type: ignore
        return False

    eel.show_toast("success", "Connected to server!", 2000) # type: ignore
    return True

@eel.expose
def login_to_server(username, password):
    """

    The `login_to_server` function logs a user into a server using a provided username and password, and
    performs additional actions based on the server's response.

    :param username: The username parameter is the username of the user trying to log in to the server
    :param password: The password parameter is the password that the user enters to log in to the server
    :return: a boolean value. If the login is successful, it returns True. If the login fails, it returns False.

    """

    global api
    response = api.login(username, password)
    print(response)
    if response["result"] == "ok":
        messages = connect_to_db(username)
        if messages is  None:
            print("No messages DB connected!")
        disconnect_from_db(messages)
        eel.show_toast("success", "Logged in successfully!", 2000) # type: ignore
        return True
    else:
        eel.show_toast("danger", response["message"], 2000) # type: ignore
        return False

@eel.expose
def register_to_server(username, password):
    """
    This function registers a new user with the given username and password.

    :param username: The username of the new user.
    :type username: str
    :param password: The password for the new user.
    :type password: str
    :return: True if registration is successful, False otherwise.
    :rtype: bool
    """
    global api
    response = api.register(username, password)
    print(response)
    if response["result"] == "ok":
        messages = connect_to_db(username)
        if messages is None:
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
def get_messages(username, contact):
    """
    This function retrieves all messages for a given username from a database and
    returns them as a list.

    :param username: The username of the user for whom we want to retrieve all messages.
    :type username: str
    :param contact: The contact for whom we want to retrieve all messages.
    :type contact: str
    :return: All the messages from the database for the given username or an empty list if there
             is no database connected.
    :rtype: list
    """
    messages = connect_to_db(username)
    if messages is not None:
        print("Getting messages...")
        all_messages = messages.get_messages(contact)
        disconnect_from_db(messages)
        return all_messages
    else:
        print("No messages DB connected!")
        disconnect_from_db(messages)
        return []


def connect_to_db(username):
    """
    Connects to the database using the provided username.

    :param username: The username to connect to the database.
    :type username: str
    :return: The connected database object or None if unable to connect.
    :rtype: messagesDB.MessagesDB or None
    """
    try:
        db = messagesDB.MessagesDB(username)
        return db
    except Exception as e:
        print(e)
        eel.show_toast("danger", "Unable to connect to database!", 2000) # type: ignore
        return None

def disconnect_from_db(db):
    """
    Disconnects from the database.

    :param db: The database connection object.
    :type db: object
    :return: None
    :rtype: None
    """
    if db is not None:
        db.close()
        db = None

@eel.expose
def logout_from_server():
    """
    Logs out from the server.

    Returns:
        bool: True if the logout was successful, False otherwise.
    """
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
    """
    Get the pending invites from the API.

    :return: A list of pending invites.
    :rtype: list
    """
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
    """
    Get the list of friends.

    Returns:
        list: A list of friends.
        rtype: list
    """
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
    """
    Get the list of online users.

    Returns:
        list: A list of online users.

    Raises:
        KeyError: If the response does not contain the expected keys.
    """
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
    """
    Get search users from the API.

    :param search: The search query.
    :type search: str
    :return: A list of search results.
    :rtype: list
    """
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
    """
    Sends a friend request to the specified username.

    :param username: The username of the friend to add.
    :type username: str
    :return: True if the friend request was sent successfully, False otherwise.
    :rtype: bool
    """
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
    """
    Accepts a friend request from the specified username.

    :param username: The username of the friend request to accept.
    :type username: str
    :return: True if the friend request was accepted successfully, False otherwise.
    :rtype: bool
    """
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
    """
    Rejects a friend request from the specified username.

    :param username: The username of the friend request to reject.
    :type username: str
    :return: True if the friend request was successfully rejected, False otherwise.
    :rtype: bool
    """
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
    """
    Remove a friend from the chat application.

    :param username: The username of the friend to be removed.
    :type username: str
    :return: True if the friend is successfully removed, False otherwise.
    :rtype: bool
    """
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
    """
    Send a message from the sender to the recipient.

    :param sender: The sender of the message.
    :type sender: str
    :param recipient: The recipient of the message.
    :type recipient: str
    :param message: The message to be sent.
    :type message: str
    :return: True if the message was sent successfully, False otherwise.
    :rtype: bool
    """
    global api
    try:
        response = api.send(recipient, message)
        print(response)
        if response["result"] == "ok":
            # Add message to DB
            messages = connect_to_db(sender)
            if messages is not None:
                messages.insert_message(recipient, message, True)
                eel.update_messages(messages.get_messages(recipient)) # type: ignore
            else:
                print("No messages DB connected!")

            disconnect_from_db(messages)
            return True
        else:
            eel.show_toast("danger", response["message"], 2000)  # type: ignore
            return False
    except Exception as e:
        print(e)
        return False


def initProjectPath():
    """
    Initialize the project path.

    This function sets the project directory to the directory of the current script.

    :return: None
    """
    script_directory = os.path.dirname(os.path.abspath(__file__))
    # Use the script_directory as the project directory
    project_directory = script_directory
    os.chdir(project_directory)


def startApp():
    """
    Starts the web application using Eel.

    This function initializes the Eel web app and starts the index.html file.

    Raises:
        SystemExit: If the application encounters a system exit error.
        MemoryError: If the application encounters a memory error.
        KeyboardInterrupt: If the application is interrupted by a keyboard interrupt.

    Returns:
        int: 0 if the application exits successfully.
    """

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

