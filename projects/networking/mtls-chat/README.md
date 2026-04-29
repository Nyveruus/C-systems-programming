# mTLS Chat App

Secure communication over TLS using the OpenSSL library between a TCP server and up to 100 clients. Both sides must present valid certificates issued by a common CA, hence "mutual" TLS (mTLS). Server multiplexes all sockets and stdin using poll(); client connections are simultaneously tracked and properly cleaned up on disconnect. Rejects any attempts to downgrade below TLS 1.3. Upon successful connection, clients print the TLS session ticket and the server prints the IP address of the newly connected client.

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

