//listen for inocming connections poll, complete tcp handshake, tls handshake, poll, read
//create ssl object

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "server.h"

#define BACKLOG 10
#define MAX_CLIENTS 100
#define TIMEOUT 2000

void setup_context(SSL_CTX **ctx, char *ca, char *cert, char *key);
void setup_tcp(int *socket_fd, char *ip, int port);
void poll_loop(int socket_fd, SSL_CTX *ctx);
int tcp_accept_function(int socket_fd, struct pollfd *fds, int *nfds, int *out_index);
int tls_accept_function(SSL **ssls, SSL_CTX *ctx, struct pollfd *fds, int out_index);

int server(char *ip, int port, char *ca, char *cert, char *key) {
    SSL_CTX *ctx;
    int socket_fd;

    setup_context(&ctx, ca, cert, key);

    setup_tcp(&socket_fd, ip, port);

    poll_loop(socket_fd, ctx);

    SSL_CTX_free(ctx);
    return 0;
}

void setup_context(SSL_CTX **ctx, char *ca, char *cert, char *key) {
    if (!(*ctx = SSL_CTX_new(TLS_server_method()))) {
        perror("SSL_CTX_new");
        exit(1);
    }

    if (!SSL_CTX_load_verify_locations(*ctx, ca, NULL)) {
        perror("set ca");
        exit(1);
    }

    SSL_CTX_set_client_CA_list(*ctx, SSL_load_client_CA_file(ca));

    if (!SSL_CTX_use_certificate_file(*ctx, cert, SSL_FILETYPE_PEM)) {
        perror("set certificate");
        exit(1);
    }
    if (!SSL_CTX_use_PrivateKey_file(*ctx, key, SSL_FILETYPE_PEM)) {
        perror("Private key");
        exit(1);
    }
    if (!SSL_CTX_check_private_key(*ctx)) {
        perror("Private key and certificate don't match");
        exit(1);
    }

    SSL_CTX_set_verify(*ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(*ctx, 1);

    return;
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

void poll_loop(int socket_fd, SSL_CTX *ctx) {
    struct pollfd fds[MAX_CLIENTS];
    SSL *ssls[MAX_CLIENTS];
    int number_revents, nfds = 2;

    fds[0].fd = socket_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;
    fds[1].revents = 0;

    for (int i = 2; i < MAX_CLIENTS; i++) {
        fds[i].fd = -1;
    }

    for (;;) {
        number_revents = poll(fds, nfds, TIMEOUT);
        //accept connections
        if (number_revents > 0 && (fds[0].revents & POLLIN)) {
            //tcp
            int out_index;
            if (tcp_accept_function(socket_fd, fds, &nfds, &out_index) == 1)
                continue;
            //tls, if tls fails, close tcp client socket and reset poll with out_index
            if (tls_accept_function(ssls, ctx, fds, out_index) == 1) {
                close(fds[out_index].fd);
                fds[out_index].fd = -1;
                continue;
            }
        }
        //read and write STDIN_FILENO
        //read from clients (detect disconnects and close fds, free memory)
    }
}

int tcp_accept_function(int socket_fd, struct pollfd *fds, int *nfds, int *out_index) {
    struct sockaddr_in client;
    socklen_t addr_len = sizeof(struct sockaddr);
    int client_fd;

    client_fd = accept(socket_fd, (struct sockaddr *)&client, &addr_len);
    if (client_fd < 0) {
        perror("accept");
        return 1;
    }
    for (int i = 2; i < MAX_CLIENTS; i++)
        if (fds[i].fd == -1) {

            fds[i].fd = client_fd;
            fds[i].events = POLLIN;
            fds[i].revents = 0;

            if (i >= *nfds)
                //+1 because of index
                *nfds = i + 1;
            *out_index = i;
            return 0;
        }
    fprintf(stderr, "max clients reached\n");
    close(client_fd);
    return 1;
}

int tls_accept_function(SSL **ssls, SSL_CTX *ctx, struct pollfd *fds, int out_index) {
    if (!(ssls[out_index] = SSL_new(ctx)))
        return 1;
    if (!SSL_set_fd(ssls[out_index], fds[out_index].fd)) {
        perror("SSL_set_fd");
        SSL_free(ssls[out_index]);
        return 1;
    }
    int r;
    if ((r = SSL_accept(ssls[out_index])) <= 0) {
        perror("SSL_accept");
        if (r < 0) {
            SSL_shutdown(ssls[out_index]);
        }
        SSL_free(ssls[out_index]);
        return 1;
    }

    return 0;
}
