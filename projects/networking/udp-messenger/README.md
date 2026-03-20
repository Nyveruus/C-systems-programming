# UDP Messenger

Simple program that sends a user message over UDP 10 times. It takes three arguments: IP, port, and message


## Architecture

The program checks for four arguments and then creates a datagram socket. In a separate function it attempts to connect to the address and port. If connection succeeds, it writes the message to the socket, iterating a hard coded amount of times

## Usage

```
$ ./main <IP> <PORT> <MESSAGE>
```

## Compilation

```
$ gcc main.c -o main
```

## Example
```
./main 127.0.0.1 8080 "Hello world"
```

```
$ nc -ul 127.0.0.1 -p 8080
Hello world
Hello world
Hello world
Hello world
Hello world
Hello world
Hello world
Hello world
Hello world
Hello world
```
