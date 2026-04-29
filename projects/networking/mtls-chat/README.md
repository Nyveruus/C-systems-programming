# mTLS Chat App

Secure communication over TLS using the OpenSSL library between a TCP server and up to 100 clients. Server broadcasts and clients unicast to server. Both sides must present valid certificates issued by a common CA, hence "mutual" TLS (mTLS). Server multiplexes all sockets and stdin using poll(); client connections are simultaneously tracked and properly cleaned up on disconnect. Rejects any attempts to downgrade below TLS 1.3. Upon successful connection, clients print the TLS session ticket and the server prints the IP address of the newly connected client.

Select server or client mode at run time

This is a continuation and improvement of [TCP Suite](https://github.com/Nyveruus/systems-programming/tree/main/projects/networking/tcp-suite)

## Build & Usage

```
$ make
$ ./mtlsapp server|client IP PORT CA CERT KEY
```

## Architecture

![architecture](docs/architecture.jpeg)

main.c is in charge of printing usage and calling the right function depending on the mode. It checks for at least 7 arguments and strcmps argv[1] to either "server" or "client". The next 5 arguments get passed to the function: Internet Protocol address & port to bind to/connect to (TCP), Certificate Authority's certificate to verify peer's certificate, own certificate signed by the Certificate Authority, and private key to sign own certificate (TLS)

### Server

server() works as a wrapper function, calling the functions that perform setup and the main loop, it also declares the socket_fd for listening for new connections and the SSL context variable to pass around and modify in reference between the functions. Setting up the context (setup_context) involves all the steps that make TLS secure: getting a new SSL_CTX pointer, loading the CA cert, the certificate file to use, private key for signing, and  various config options like verifying peer and setting a minimum protocol version TLS 1.3. After setting up the context and returning, tcp and the listening socket is set up (setup_tcp). The final function (poll_loop) declares two parallel arrays: struct pollfd *fds (for the actual file descriptors) and SSL **ssls (tls objects when reading and writting); the inactive fds that are reserved for clients are set to -1. In the poll loop, connections and activity are monitored through fds and the index of fds is used for indexing into SSL **ssls when necessary. fds[0] and fds[1] are reserved for the socket that is accepting new connections and for stdin. 

The poll loop has 3 jobs: accepting new connections, reading & writing STDIN & monitoring clients

- Accepting new connections - TCP: getting a client socket file descriptor, assigning it to first fds.fd that isn't -1 and recording index. TLS: uses index from TCP to assign a new SSL object in the SSL array, uses the index to set the file decriptor the SSL object and uses the index to accept the connection. The TCP accept function has error handling if there are no more free fds slots, and the TLS accept function shutsdown and frees memory as necessary on error too and returns 1, causing fds and nfds to cleanup and close the connection.

- Reading & Writing STDIN - Reads to a buffer, continues on error, broadcasts with SSL_WRITE if successful.

- Monitor - checks for activity on client file descriptor and attempts to SSL_read into buffer. If > 0 is returned, writes to STDOUT, otherwise a switch and SSL_get_error is used to handle disconnections and errors (cleanup)

### Client

Works similar in structure as its server counterpart, except without fds and ssls * arrays; attempts connection once within a merged tcp_tls_accept() function. setup_context additionaly sets up a SSL session callback function for printing the TLS ticket once connection succeeds. It is important to set the socket to non blocking after the handshake succeeds.

## Generate Certificates & Keys

**CA**: private key & self-signed certificate

```
$ openssl req \
    -x509 \
    -nodes \
    -days 365 \
    -subj "/CN=ca" \
    -newkey rsa \
    -keyout ca_key.pem \
    -out ca_cert.pem
```
**Server**: private key & CSR, CA generates certificate

```
$ openssl genrsa -out server_key.pem
$ openssl req -new \
    -key server_key.pem \
    -subj "/CN=server" \
    -out server_csr.pem
$ openssl x509 \
    -req \
    -days 365 \
    -in server_csr.pem \
    -CA ca_cert.pem \
    -CAkey ca_key.pem \
    -CAcreateserial \
    -out server_cert.pem
```

**Client**: private key & CSR, CA generates certificate

```
$ openssl genrsa -out client1_key.pem
$ openssl req -new \
    -key client1_key.pem \
    -subj "/CN=client1" \
    -out client1_csr.pem
$ openssl x509 \
    -req \
    -days 365 \
    -in client1_csr.pem \
    -CA ca_cert.pem \
    -CAkey ca_key.pem \
    -CAcreateserial \
    -out client1_cert.pem
```

