//listen for inocming connections poll, complete tcp handshake, tls handshake, poll, read
//create ssl object

#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>

#include "server.h"

int server(int port, char *ca, char *cert, char *key) {
    SSL_CTX *ctx;
    setup_object(&ctx);

    SSL_CTX_free(ctx);
    return 0;
}

void setup_object(SSL_CTX **ctx) {
    if (!(*ctx = SSL_CTX_new(TLS_server_method()))) {
        perror("SSL_CTX_new");
        exit(1);
    }
}
