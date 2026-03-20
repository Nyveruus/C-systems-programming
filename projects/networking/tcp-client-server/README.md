# The Program

This program implements a TCP server and client pair with bounds checking. Clients can send data from their standard input (stdin) to the server and read data from the server. The server can read from all connected clients and broadcast data from its own standard input to all clients, functioning as a sort of chat app

The server and client take two arguments each: IP and port

## Architecture

The server program creates a server socket, binds to an address and port from arguments, and listens on it. Then it enters an infinite poll loop monitoring three sources: the server socket for new connections (up to 100 clients), stdin for input to broadcast to all connected clients, and each client socket for incoming data to print to stdout. Errors and cleanup are handled gracefully by closing all open file descriptors. The incoming connections are accepted and tracked in a pollfd array, indices 0 and 1 are reserved for the server socket and stdin. Data received from any client is written to stdout and disconnections are detected with zero byte reads. 

The client connects to a server by IP and port, then enters a poll loop monitoring the server socket and stdin. Data from the server is written to stdout; stdin input is forwarded to the server. Connection and I/O logic are split into separate functions here, with the socket closed on any error or exit.

## Usage
### Server

```
$ ./server <IP> <PORT>
```

### Client

```
$ ./client <IP> <PORT>
```

## Compilation
### Server

```
$ gcc server.c -Wall -Wextra -o server
```

### Client

```
$ gcc client.c -Wall -Wextra -o client
```

## Example
### Server
```
$ ./server 127.0.0.1 8080

[client connected: 127.0.0.1]

[client connected: 127.0.0.1]

This is from client 1
This is from client 2
This is a server broadcast

[client disconnected]

[client disconnected]
```
### Clients
#### Client 1
```
$ ./client 127.0.0.1 8080
This is from client 1
This is a server broadcast
^C
```
#### Client 2
```
$ ./client 127.0.0.1 8080
This is from client 2
This is a server broadcast
^C
```
