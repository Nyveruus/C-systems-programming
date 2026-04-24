#include <stdlib.h>
#include <openssl/ssl.h>

#include "client.h"

void setup_context(SSL_CTX **ctx, char *ca, char *cert, char *key);

int client(char *ip, int port, char *ca, char *cert, char *key) {
    SSL_CTX *ctx;
    int socket_fd;

    setup_context(&ctx, ca, cert, key);

    tcp_connect(socket_fd, ip, port);

    SSL_CTX_free(ctx);
    return 0;
}

void setup_context(SSL_CTX **ctx, char *ca, char *cert, char *key) {
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

    SSL_CTX_set_verify(*ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(*ctx, 1);

    return;
}

void tcp_connect() {

}
