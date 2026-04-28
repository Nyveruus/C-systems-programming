# mTLS Chat App

Secure communication over TLS using the openssl library between a TCP server and up to 100 clients. Both sides must present valid certificates issued by a common CA, hence "mutual" TLS (mTLS). Server multiplexes all sockets and stdin using poll(); client connections are simultaneously tracked and properly cleaned up on disconnect. Rejects any attempts to downgrade below TLS 1.3. Upon successful connection, clients print the TLS session ticket and the server prints the IP address of the newly connected client.

Select server or client mode at run time

This is a continuation and improvement of [TCP Suite](https://github.com/Nyveruus/systems-programming/tree/main/projects/networking/tcp-suite)

## Architecture

![architecture](docs/architecture.jpeg)

## Usage

