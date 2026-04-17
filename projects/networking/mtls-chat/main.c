//check if server or client in argument, call client or server functions respectively

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"

void usage(void) {
    fprintf(stderr, "Usage: ./mtlsapp server [PORT] [CA] [CERT] [KEY]\n");
    return;
}

int select_function(int argc, char **argv) {
    if (!strcmp(argv[1], "server"))
        return server(atoi(argv[2]), argv[3], argv[4], argv[5]);
    else if (!strcmp(argv[1], "client"))
        return 1; //TODO client function
    else
        usage();
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        usage();
        return 1;
    }
    select_function(argc, argv);
}

