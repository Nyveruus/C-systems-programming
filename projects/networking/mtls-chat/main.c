//check if server or client in argument, call client or server functions respectively

#include <stdio.h>
#include <string.h>

int select(int argc, char **argv) {
    if (!strcmp(argv[1], "server"))
        return server();
    else if (!strcmp(argv[1], "client"))
        return; //client function
    else
        usage();
}

void usage(void) {
    fprintf(stderr, "Usage: ./mtlsapp server|client\n");
    return;
}

int main (int argc, char *argv[]) {
    if (argc < 2) {
        usage();
        return 1;
    }
    select(argc, argv);
}

