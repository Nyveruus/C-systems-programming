//listen for inocming connections poll, complete tcp handshake, tls handshake, poll, read
//create ssl object

#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"

void setup_object(SSL_CTX **ctx);
void setup_tcp(int *socket_fd, char *ip, int port);

int server(char *ip, char *port, char *ca, char *cert, char *key) {
    SSL_CTX *ctx;
    setup_object(&ctx);

    port = atoi(port);
    int socket_fd;
    setup_tcp(&socket_fd, ip, port);

    SSL_CTX_free(ctx);
    return 0;
}

void setup_object(SSL_CTX **ctx) {
    if (!(*ctx = SSL_CTX_new(TLS_server_method()))) {
        perror("SSL_CTX_new");
        exit(1);
    }
}

void setup_tcp(int *socket_fd, char *ip, int port) {
    struct sockaddr_in server;
    socklen_t addr_size;

    if ((*socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons((uint16_t)port);
    server.sin_addr.s_addr = inet_addr(ip);
}
