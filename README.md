# Server Documentation
![Build CI](https://github.com/kubsnn/TCP-Chat/actions/workflows/ci.yml/badge.svg) </br>
This documentation provides information about the available requests for the TCP Chat server. Each request must be in JSON format.

## Data Format Requirements

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

* "action" must be set to "register".
* "creds" should contain the user's credentials with "username" and "password" fields.


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
* "response" will be true to indicate a response.
* "action" will be set to "register" to indicate a registration response.
* "result" will be set to "ok" if the registration is successful or "fail" in case of an error.
* "message" (optional) will provide a failure reason in case of registration failure.


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
* "action" must be set to "login".
* "creds" should contain the user's credentials with "username" and "password" fields.

  
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
* "response" will be true to indicate a response.
* "action" will be set to "login" to indicate a login response.
* "result" will be set to "ok" if the login is successful or "fail" in case of an error.
* "message" (optional) will provide a failure reason in case of a login failure.

## Online Users Request
To get list of online users, client must be logged in and send following request:
```json
{
  "action" : "usersOnline"
}
```

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

* "response" will be true to indicate a response.
* "action" will be set to "usersOnline" to indicate a usersOnline response.
* "result" will be set to "ok" if the request is successful or "fail" in case of an error.
* "values" (on success) will contain a list of online users.
* "message" (on fail) will provide a failure reason in case of a request failure.

## Send Message Request
To send message to an online user, client must be logged in and send following request:
```json
{
  "action" : "sendto",
  "who" : "<USERNAME>",
  "message" : "<DATA...>"
}
```

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
* "response" will be true to indicate a response.
* "action" will be set to "sendto" to indicate a sendto response.
* "result" will be set to "ok" if the request is successful or "fail" in case of an error.
* "message" (optional) will provide a failure reason in case of a request failure.

