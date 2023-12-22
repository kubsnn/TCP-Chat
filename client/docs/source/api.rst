Python TCP API
==============

API Class Methods:


==================== ========================================================================================================
Method                Description
==================== ========================================================================================================
__init__()            Initializes a API object.
connect()             Connects to the server.
close()               Closes the connection with the server.

register()            Sends a registration request to the server with the provided username and password.
login()               Sends a login request to the server with the provided username and password.
logout()              Sends a logout request to the server.

search()              Sends a search request to the server to find a user with the provided username.
getOnlineUsers()      Sends a request to the server to get a list of online users.
invite()              Sends an invitation request to the server to invite a user with the provided username.
accept()              Sends an accept request to the server to accept an invitation from a user with the provided username.
invitations()         Sends a request to the server to get a list of pending invitations from other users.
friends()             Sends a request to the server to get a list of friends.
remove()              Sends a request to the server to remove a friend.

send()                Sends a message to the specified receiver.
==================== ========================================================================================================


.. automodule:: src.api
   :members:
   :undoc-members:
   :show-inheritance:
