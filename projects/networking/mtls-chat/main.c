//check if server or client in argument, call client or server functions respectively

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"
#include "client.h"

void usage(void) {
    fprintf(stderr, "Usage: ./mtlsapp server|client [IP] [PORT] [CA] [CERT] [KEY]\n");
    return;
}

int select_function(char **argv) {
    if (!strcmp(argv[1], "server"))
        return server(argv[2], atoi(argv[3]), argv[4], argv[5], argv[6]);
    else if (!strcmp(argv[1], "client"))
        return client(argv[2], atoi(argv[3]), argv[4], argv[5], argv[6]);
    else
        usage();
    return 1;

}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        usage();
        return 1;
    }
    select_function(argv);
}

