# Server Documentation

![Build CI](https://github.com/kubsnn/TCP-Chat/actions/workflows/ci.yml/badge.svg)
![Tests CI](https://github.com/kubsnn/TCP-Chat/actions/workflows/ci_tests.yml/badge.svg) </br>

## Dependencies

Before building and running the project, ensure that the following dependencies are installed on your system:

- **GCC and G++ >= 10:**

    ```bash
    sudo apt-get update
    sudo apt-get install gcc
    sudo apt-get install g++
    ```

- **OpenSSL:**

    ```bash
    sudo apt-get install libssl-dev
    ```

- **CMake:**

    ```bash
    sudo apt-get install cmake
    ```

- **Make:**

    ```bash
    sudo apt-get install make
    ```

- **SQLite:**

    ```bash
    sudo apt-get install sqlite3
    sudo apt-get install libsqlite3-dev
    ```

And for testing:

- **GTest:**

    ```bash
    sudo apt-get install libgtest-dev
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    ```

Please ensure all dependencies are installed before proceeding with the build and execution of the project.

## Build

To build the project, run the following commands from the /server directory of the project:

```bash
cmake .
make
```

## Run

To run the server, run the following command from the /server/bin:

```bash
./app [ip] [port] [--debug]
```

- "ip" must be set to the ip address of the server. (default: 0.0.0.0)
- "port" must be set to the port of the server. (default: 1100)
- "--debug" (optional) must be set to enable debug mode. (default: disabled)
- Note: arguments don't have to be passed in the order specified above.

### Example

```bash
./app 127.0.0.1 8080
```

```bash
./app --debug 8080
```

## Data Format Requirements

This documentation provides information about the available requests for the TCP Chat server. Each request must be in JSON format.

### Encryption

All data sent to the server must be encrypted using 2048 bit RSA public key that server provides on connection.

On connection start server sends 2048 bit RSA public key in PEM format in JSON with the following format:

```json
{
  "public_key" : "-----BEGIN RSA PUBLIC KEY-----\n...",
  "message" : "Waiting for your public key...",
  "action" : "init"
}
```

And awaits for client's response already encrypted with server public key:

```json
{
  "public_key" : "-----BEGIN RSA PUBLIC KEY-----\n...",
  "action" : "init"
}
```

Server will continue to send its public key until it receives valid json from the client.

### Format

To send JSON data to the server, the client must adhere to the following format:

1. **Data Length:** First, the client should write 4 bytes (an integer) that represents the length of the encrypted string of parsed JSON.

2. **Encrypted Parsed JSON:** Following the data length, the client should send the encrypted string in blocks of 256 bytes. (Because the RSA key is 2048 bits, or 256 bytes, long.)

If data length is suspiciously big (n > 65536) server ignores all bytes

## Register Request

To register a new user, send a JSON request to the server with the following format:

```json
{
  "action" : "register",
  "creds" : {
    "username" : "<USERNAME>",
    "password" : "<PASSWORDHASH>"
  }
}
```

- "action" must be set to "register".
- "creds" should contain the user's credentials with "username" and "password" fields.

## Register Response

The server will respond with a JSON object in the following format:

```json
{
  "response" : true,
  "action" : "register",
  "result" : "ok or fail",
  "message" : "failure reason" (optional)
}
```

- "response" will be true to indicate a response.
- "action" will be set to "register" to indicate a registration response.
- "result" will be set to "ok" if the registration is successful or "fail" in case of an error.
- "message" (optional) will provide a failure reason in case of registration failure.

## Login Request

To log in, send a JSON request to the server with the following format:

```json
{
  "action" : "login",
  "creds" : {
    "username" : "<USERNAME>",
    "password" : "<PASSWORDHASH>"
  }
}
```

- "action" must be set to "login".
- "creds" should contain the user's credentials with "username" and "password" fields.

## Login Response

The server will respond with a JSON object in the same format as the register response:

```json
{
  "response" : true,
  "action" : "login",
  "result" : "ok or fail",
  "message" : "failure reason" (optional)
}
```

- "response" will be true to indicate a response.
- "action" will be set to "login" to indicate a login response.
- "result" will be set to "ok" if the login is successful or "fail" in case of an error.
- "message" (optional) will provide a failure reason in case of a login failure.

## Online Users Request

To get list of online users, client must be logged in and send following request:

```json
{
  "action" : "usersOnline"
}
```

- "action" must be set to "usersOnline".

## Online Users Response

The server will respond with a JSON object in the following format:

```json
{
  "response" : true,
  "action" : "usersOnline",
  "result" : "ok or fail",
  "values" : [ "<USERNAMES>", ... ], (if ok)
  "message" : "failure reason" (if fail)
}
```

- "response" will be true to indicate a response.
- "action" will be set to "usersOnline" to indicate a usersOnline response.
- "result" will be set to "ok" if the request is successful or "fail" in case of an error.
- "values" (on success) will contain a list of online users.
- "message" (on fail) will provide a failure reason in case of a request failure.

## Send Message Request

To send message to an online user, client must be logged in and send following request:

```json
{
  "action" : "sendto",
  "who" : "<USERNAME>",
  "message" : "<DATA...>"
}
```

- "action" must be set to "sendto".
- "who" must be set to the username of the recipient (must be online and your friend).
- "message" must be set to the message to be sent.

## Send Message Response

The server will respond with a JSON object in the following format:

```json
{
  "response" : true,
  "action" : "sendto",
  "result" : "ok or fail",
  "message" : "failure reason" (optional)
}
```

- "response" will be true to indicate a response.
- "action" will be set to "sendto" to indicate a sendto response.
- "result" will be set to "ok" if the request is successful or "fail" in case of an error.
- "message" (optional) will provide a failure reason in case of a request failure.

## Invite Friend Request

To invite a user to a friend list, client must be logged in and send following request:

```json
{
  "action" : "invite",
  "who" : "<USERNAME>"
}
```

- "action" must be set to "invite".
- "who" must be set to the username of the recipient.

## Invite Friend Response

The server will respond with a JSON object in the following format:

```json
{
  "response" : true,
  "action" : "invite",
  "result" : "ok or fail",
  "message" : "failure reason" (optional)
}
```

- "response" will be true to indicate a response.
- "action" will be set to "invite" to indicate a invite response.
- "result" will be set to "ok" if the request is successful or "fail" in case of an error.
- "message" (optional) will provide a failure reason in case of a request failure.

## Accept Friend Request

To accept a friend request, client must be logged in and send following request:

```json
{
  "action" : "accept",
  "who" : "<USERNAME>"
}
```

- "action" must be set to "accept".
- "who" must be set to the username of the recipient.

## Accept Friend Response

The server will respond with a JSON object in the following format:

```json
{
  "response" : true,
  "action" : "accept",
  "result" : "ok or fail",
  "message" : "failure reason" (optional)
}
```

- "response" will be true to indicate a response.
- "action" will be set to "accept" to indicate a accept response.
- "result" will be set to "ok" if the request is successful or "fail" in case of an error.
- "message" (optional) will provide a failure reason in case of a request failure.

## Get Friend List Request

To get a friend list, client must be logged in and send following request:

```json
{
  "action" : "friends"
}
```

- "action" must be set to "friends".

## Get Friend List Response

The server will respond with a JSON object in the following format:

```json
{
  "response" : true,
  "action" : "friends",
  "result" : "ok or fail",
  "values" : [ { "username" : "<USERNAME>", "online" : true/false }, ...], (if ok)
  "message" : "failure reason" (if fail)
}
```

- "response" will be true to indicate a response.
- "action" will be set to "friends" to indicate a friends response.
- "result" will be set to "ok" if the request is successful or "fail" in case of an error.
- "values" (on success) will contain a list of friends.
- "message" (on fail) will provide a failure reason in case of a request failure.

## Get Invitations Request

To get a list of invitations, client must be logged in and send following request:

```json
{
  "action" : "invitations"
}
```

- "action" must be set to "invitations".

## Get Invitations Response

The server will respond with a JSON object in the following format:

```json
{
  "response" : true,
  "action" : "invitations",
  "result" : "ok or fail",
  "values" : [ { "username" : "<USERNAME>" }, ...], (if ok)
  "message" : "failure reason" (if fail)
}
```

- "response" will be true to indicate a response.
- "action" will be set to "invitations" to indicate a invitations response.
- "result" will be set to "ok" if the request is successful or "fail" in case of an error.
- "values" (on success) will contain a list of invitations.
- "message" (on fail) will provide a failure reason in case of a request failure.

# Remove Friend Request

To remove a friend from a friend list, client must be logged in and send following request:

```json
{
  "action" : "removeFriend",
  "who" : "<USERNAME>"
}
```

- "action" must be set to "removeFriend".
- "who" must be set to the username of the recipient.

## Remove Friend Response

The server will respond with a JSON object in the following format:

```json
{
  "response" : true,
  "action" : "removeFriend",
  "result" : "ok or fail",
  "message" : "failure reason" (optional)
}
```
