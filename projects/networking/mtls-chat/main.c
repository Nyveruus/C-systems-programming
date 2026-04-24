//check if server or client in argument, call client or server functions respectively

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"
#include "client.h"

void usage(void) {
    fprintf(stderr, "Usage: ./mtlsapp server [IP] [PORT] [CA] [CERT] [KEY]\n");
    return;
}

int select_function(char **argv) {
    if (!strcmp(argv[1], "server"))
        server(argv[2], atoi(argv[3]), argv[4], argv[5], argv[6]);
    else if (!strcmp(argv[1], "client"))
        client(argv[2], atoi(argv[3]), argv[4], argv[5], argv[6]);
    else
        usage();
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        usage();
        return 1;
    }
    select_function(argv);
}

