
# Server Documentation

This documentation provides information about the available requests for the TCP Chat server. Each request must be in JSON format.

## Data Format Requirements

To send JSON data to the server, the client must adhere to the following format:

1. **Data Length:** First, the client should write 4 bytes (an integer) that represents the length of the parsed JSON as a string.

2. **Parsed JSON:** Following the data length, the client should send the parsed JSON data.

If data length is suspiciously big (n > 65536) server ignores all bytes

## Register Request

To register a new user, send a JSON request to the server with the following format:

```json
{
  "action": "register",
  "creds": {
    "username": "<USERNAME>",
    "password": "<PASSWORDHASH>"
  }
}
```

* "action" must be set to "register".
* "creds" should contain the user's credentials with "username" and "password" fields.


## Register Response
The server will respond with a JSON object in the following format:

```json
{
  "response": true,
  "result": "ok or fail",
  "message": "failure reason" (optional)
}
```
* "response" will be true to indicate a response.
* "result" will be set to "ok" if the registration is successful or "fail" in case of an error.
* "message" (optional) will provide a failure reason in case of registration failure.


## Login Request
To log in, send a JSON request to the server with the following format:

```json
{
  "action": "login",
  "creds": {
    "username": "<USERNAME>",
    "password": "<PASSWORDHASH>"
  }
}
```
* "action" must be set to "login".
* "creds" should contain the user's credentials with "username" and "password" fields.

  
## Login Response
The server will respond with a JSON object in the same format as the register response:

```json
{
  "response": true,
  "result": "ok or fail",
  "message": "failure reason" (optional)
}
```
* "response" will be true to indicate a response.
* "result" will be set to "ok" if the login is successful or "fail" in case of an error.
* "message" (optional) will provide a failure reason in case of a login failure.

