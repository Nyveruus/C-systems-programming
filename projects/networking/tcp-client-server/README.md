# The Program

This program implements a TCP server and client pair with bounds checking. Clients can send data from their standard input (stdin) to the server and read data from the server. The server can read from all connected clients and broadcast data from its own standard input to all clients.

## Architecture
### Server
The program creates a server socket, binds and listens on it, then enters an infinite poll loop monitoring three sources: the server socket for new connections (up to 100 clients), stdin for input to broadcast to all connected clients, and each client socket for incoming data to print to stdout. Errors and cleanup are handled gracefully by closing all open file descriptors.

The incoming connections are accepted and tracked in a pollfd array. Data received from any client is written to stdout and  disconnections are detected with zero byte reads. 
### Client

## Usage
### Server
### Client

## Compilation
### Server
### Client

## Example
### Server
### Client
