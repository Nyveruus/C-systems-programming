//listen for inocming connections poll, complete tcp handshake, tls handshake, poll, read
//create ssl object

#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "server.h"

#define BACKLOG 10
#define MAX_CLIENTS 100
#define TIMEOUT 2000

void setup_object(SSL_CTX **ctx);
void setup_tcp(int *socket_fd, char *ip, int port);
int poll_loop(int socket_fd, char *ca, char *cert, char *key);

int server(char *ip, char *port, char *ca, char *cert, char *key) {
    SSL_CTX *ctx;
    port = atoi(port);
    int socket_fd;

    setup_object(&ctx);

    setup_tcp(&socket_fd, ip, port);

    poll_loop(socket_fd, ca, cert, key);

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
    socklen_t addr_size = sizeof(struct sockaddr);

    if ((*socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset(&server, 0, addr_size);
    server.sin_family = AF_INET;
    server.sin_port = htons((uint16_t)port);
    server.sin_addr.s_addr = inet_addr(ip);

    if (bind(*socket_fd, (struct sockaddr *)&server, addr_size) == -1) {
        perror("Bind");
        exit(1);
    }
    if (listen(*socket_fd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
}

int poll_loop(int socket_fd, char *ca, char *cert, char *key) {
    struct pollfd fds[MAX_CLIENTS + 2];
    struct sockaddr_in peer;
    int nfds = 2;

    fds[0].fd = socket_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;
    fds[1].revents = 0;

    for (int i = 2; i <= MAX_CLIENTS; i++) {
        fds[i].fd = -1;
    }


}
