#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <poll.h>
#include <fcntl.h>

#include "client.h"

#define TIMEOUT 2000
#define TLS_RECORD_SIZE 16384

static void setup_context(SSL_CTX **ctx, char *ca, char *cert, char *key);
static void tcp_tls_connect(int *socket_fd, char *ip, int port, SSL_CTX *ctx, SSL **sslo);
static void poll_loop(int socket_fd, SSL_CTX *ctx, SSL *sslo);
static void monitor(SSL *sslo);
static int read_client(char *buffer, size_t *total, struct pollfd *fds);

int new_session_cb(SSL *sslo, SSL_SESSION *ses) {
    SSL_SESSION_print_fp(stdout, ses);
    return 0;
}

int client(char *ip, int port, char *ca, char *cert, char *key) {
    int socket_fd;
    SSL_CTX *ctx;
    SSL *sslo;

    setup_context(&ctx, ca, cert, key);

    tcp_tls_connect(&socket_fd, ip, port, ctx, &sslo);

    poll_loop(socket_fd, ctx, sslo);

    SSL_CTX_free(ctx);
    return 0;
}

static void setup_context(SSL_CTX **ctx, char *ca, char *cert, char *key) {
    if (!(*ctx = SSL_CTX_new(TLS_client_method()))) {
        perror("SSL_CTX_new");
        exit(1);
    }

    if (!SSL_CTX_load_verify_locations(*ctx, ca, NULL)) {
        perror("set ca");
        exit(1);
    }

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

    //set caching for ticket
    SSL_CTX_set_session_cache_mode(*ctx, SSL_SESS_CACHE_CLIENT);
    //set callback function to print ticket when session is negotiated
    SSL_CTX_sess_set_new_cb(*ctx, new_session_cb);

    SSL_CTX_set_verify(*ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(*ctx, 1);

    return;
}

static void tcp_tls_connect(int *socket_fd, char *ip, int port, SSL_CTX *ctx, SSL **sslo) {
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

    if (connect(*socket_fd, (struct sockaddr *)&server, addr_size) == -1) {
        perror("connect");
        exit(1);
    }
    //tls
    if (!(*sslo = SSL_new(ctx))) {
        perror("SSL_new");
        exit(1);
    }
    if (!SSL_set_fd(*sslo, *socket_fd)) {
        perror("SSL_set_fd");
        exit(1);
    }
    if (SSL_connect(*sslo) <= 0) {
        //ssl_get_error
        fprintf(stderr, "Connection failed\n");
        exit(1);
    }

    //bug fix. set fd to non blocking so SSl_write doesnt block waiting to read session ticket
    int flags = fcntl(*socket_fd, F_GETFL, 0); //get current fd flags
    fcntl(*socket_fd, F_SETFL, flags | O_NONBLOCK); //use OR to avoid overwriting
}

static void poll_loop(int socket_fd, SSL_CTX *ctx, SSL *sslo) {
    struct pollfd fds[2];
    int number_revents, nfds = 2;

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    fds[1].fd = socket_fd;
    fds[1].events = POLLIN;
    fds[1].revents = 0;

    for (;;) {
        number_revents = poll(fds, nfds, TIMEOUT);
        //stdin
        if (number_revents > 0 && (fds[0].revents & POLLIN)) {
            size_t total;
            char buffer[TLS_RECORD_SIZE];
            if (read_client(buffer, &total, fds)) continue;
            SSL_write(sslo, buffer, total);
        }
        //read from server
        if (number_revents > 0 && (fds[1].revents & POLLIN)) {
            monitor(sslo);
        }
    }
}

static void monitor(SSL *sslo) {
    char buffer[TLS_RECORD_SIZE];
    int r = SSL_read(sslo, buffer, TLS_RECORD_SIZE);

    if (r <= 0) {
        switch (SSL_get_error(sslo, r)) {

            case SSL_ERROR_ZERO_RETURN:
            case SSL_ERROR_SYSCALL:
            case SSL_ERROR_SSL:
                printf("Server disconnected\n");
                exit(1);

            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                return;

            default:
                ERR_print_errors_fp(stderr);
                exit(1);
        }
    }
    write(STDOUT_FILENO, buffer, r);
    return;
}

static int read_client(char *buffer, size_t *total, struct pollfd *fds) {
    ssize_t r = read(fds[0].fd, buffer, TLS_RECORD_SIZE);
    if (r <= 0) return 1;

    *total = (size_t)r;
    return 0;
}
