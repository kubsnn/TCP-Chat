import tcp
from readerwriterlock import rwlock
import json
import time
from typing import Any, Callable


"""
API Class Methods:

Method              | Description
------------------- | ----------------------------------------------------------------------------------------------
__init__()          | Initializes a API object.
connect()           | Connects to the server.
close()             | Closes the connection with the server.
                    |
register()          | Sends a registration request to the server with the provided username and password.
login()             | Sends a login request to the server with the provided username and password.
logout()            | Sends a logout request to the server.
                    |
search()            | Sends a search request to the server to find a user with the provided username.
getOnlineUsers()    | Sends a request to the server to get a list of online users.
invite()            | Sends an invitation request to the server to invite a user with the provided username.
accept()            | Sends an accept request to the server to accept an invitation from a user with the provided username.
invitations()       | Sends a request to the server to get a list of pending invitations from other users.
friends()           | Sends a request to the server to get a list of friends.
remove()            | Sends a request to the server to remove a friend.
                    |
send()              | Sends a message to the specified receiver.
"""
class API:
    def __init__(self, host: str, port: int, onMessage: Callable):
        """
        Initializes a API object.

        Args:
            host (str): The host address to connect to.
            port (int): The port number to connect to.
            onMessage (Callable): The callback function to handle incoming messages.
        """
        self.client = tcp.Client(host, port)
        self.responses = {}
        self.lock = rwlock.RWLockWrite()
        self.closed = False
        self.onMessage = onMessage

    def connect(self) -> bool:
        """
        Connects to the server.

        Returns:
            bool: True if the connection is successful, False otherwise.
        """
        if self.client.connect():
            self.client.set_listener(self.__handleResponse)
            return True
        return False

    def close(self):
        """
        Closes the connection with the server.
        """
        if self.closed:
            return
        self.client.exit()
        self.closed = True

    def register(self, username: str, password: str) -> dict[str, Any]:
        """
        Sends a registration request to the server with the provided username and password.

        Args:
            username (str): The username to register.
            password (str): The password for the user.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "register", "creds": {"username" : self.__encode_utf8(username), "password" : password}}
        self.client.send(json.dumps(request))
        return self.__getResponse("register")

    def login(self, username: str, password: str) -> dict[str, Any]:
        """
        Sends a login request to the server with the provided username and password.

        Args:
            username (str): The username to login.
            password (str): The password for the user.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "login", "creds": {"username" : self.__encode_utf8(username), "password" : password}}
        self.client.send(json.dumps(request))
        return self.__getResponse("login")

    def logout(self) -> dict[str, Any]:
        """
        Sends a logout request to the server.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "logout"}
        self.client.send(json.dumps(request))
        return self.__getResponse("logout")

    def search(self, username: str) -> dict[str, Any]:
        """
        Sends a search request to the server to find a user with the provided username.

        Args:
            username (str): The username to search for.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "search", "who" : self.__encode_utf8(username)}
        self.client.send(json.dumps(request))
        response = self.__getResponse("search")
        if response != None and "values" in response:
            for user in response["values"]:
                user["username"] = self.__decode_utf8(user["username"])

        return response

    def getOnlineUsers(self) -> dict[str, Any]:
        """
        Sends a request to the server to get a list of online users.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "usersOnline"}
        self.client.send(json.dumps(request))
        response = self.__getResponse("usersOnline")
        if response != None and "values" in response:
            response["values"] = [self.__decode_utf8(username) for username in response["values"]]

        return response

    def invite(self, username: str) -> dict[str, Any]:
        """
        Sends an invitation request to the server to invite a user with the provided username.

        Args:
            username (str): The username of the user to invite.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "invite", "who" : self.__encode_utf8(username)}
        self.client.send(json.dumps(request))
        return self.__getResponse("invite")

    def accept(self, username: str) -> dict[str, Any]:
        """
        Sends an accept request to the server to accept an invitation from a user with the provided username.

        Args:
            username (str): The username of the user to accept the invitation from.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "accept", "who" : self.__encode_utf8(username)}
        self.client.send(json.dumps(request))
        return self.__getResponse("accept")

    def send(self, receiver: str, message: str) -> dict[str, Any]:
        """
        Sends a message to the specified receiver.

        Args:
            receiver (str): The username of the receiver.
            message (str): The message to send.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "sendto", "who" : self.__encode_utf8(receiver), "message" : message}
        self.client.send(json.dumps(request))
        return self.__getResponse("sendto")

    def invitations(self) -> dict[str, Any]:
        """
        Sends a request to the server to get a list of pending invitations.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "invitations"}
        self.client.send(json.dumps(request))
        response = self.__getResponse("invitations")
        if response != None and "values" in response:
            for user in response["values"]:
                user["username"] = self.__decode_utf8(user["username"])

        return response

    def friends(self) -> dict[str, Any]:
        """
        Sends a request to the server to get a list of friends.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "friends"}
        self.client.send(json.dumps(request))
        response = self.__getResponse("friends")
        if response != None and "values" in response:
            for user in response["values"]:
                user["username"] = self.__decode_utf8(user["username"])

        return response

    def remove(self, username: str) -> dict[str, Any]:
        """
        Sends a request to the server to remove a friend.

        Returns:
            json: The response from the server.
        """
        request = {"action" : "removeFriend", "who" : self.__encode_utf8(username)}
        self.client.send(json.dumps(request))
        return self.__getResponse("removeFriend")

    # private:
    def __getResponse(self, action) -> dict[str, Any]:
        count = 0
        while True:
            with self.lock.gen_rlock():
                if action in self.responses:
                    response = self.responses[action]
                    break
            time.sleep(0.04)
            count += 1
            if count > 100:
                # TIMEOUT
                print("TIMEOUT")
                response = None
                break

        if response is not None:
            with self.lock.gen_wlock():
                del self.responses[action]
        else:
            response = {}

        return response

    def __handleResponse(self, data):
        if "action" in data:
            if data["action"] == "received":
                data["from"] = self.__decode_utf8(data["from"])
                data["to"] = self.__decode_utf8(data["to"])
                self.onMessage(data)
                return

            with self.lock.gen_wlock():
                self.responses[data["action"]] = data
        else:
            print("Unknown response: ", data)

    def __encode_utf8(self, string: str) -> str:
        return string.encode('unicode-escape').decode('utf-8')

    def __decode_utf8(self, string: str) -> str:
        return string.encode('utf-8').decode('unicode-escape')



# TESTS
if __name__ == '__main__':
    host = '127.0.0.1'
    port = 1100

    def receiver(data: dict):
        print("RECEIVED: ", data)

    requests = API(host, port, receiver)
    requests.connect()

    print(requests.register("Łukasz", "test"))

    print(requests.register("Jędrzej", "test"))
    print(requests.login("Jędrzej", "test"))
    res = requests.search("Łu")
    print(res)
    print(requests.invite(res["values"][0]["username"]))
    print(requests.logout())

    print(requests.login("Łukasz", "test"))
    print(requests.invitations())
    print(requests.accept("Jędrzej"))
    print(requests.friends())
    print(requests.friends())
    print(requests.remove("Jędrzej"))
    print(requests.friends())
    print(requests.logout())

    requests2 = API(host, port, receiver)
    requests2.connect()

    print(requests2.login("Jędrzej", "test"))
    print(requests.login("Łukasz", "test"))

    print(requests2.send("Łukasz", "siema"))

    requests.close()