#ifndef SERVER_H
#define SERVER_H

#include <openssl/ssl.h>
int server(int port, char *ca, char *cert, char *key);
void setup_object(SSL_CTX *object);

#endif
