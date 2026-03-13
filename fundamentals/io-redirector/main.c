//fork
//open
//dup2
//close
//exec

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

typedef enum {
    OUT,
    OUT_A,
    IN,
    UNKNOWN
} redirect_opts;

redirect_opts parse_op(const char *op) {
    if (strcmp(op, ">") == 0) return OUT;
    if (strcmp(op, ">>") == 0) return OUT_A:
    if (strcmp(op, "<") == 0) return IN;
    return UNKNOWN;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: ./main cmd >|>>|< file");
        return 1;
    }
    const char *cmd = argv[1];
    const char *operator = argv[2];
    const char *file = argv[3];
    switch (parse_op(operator)) {
        case OUT:
            run();
            break;
        case OUT_A:
            run();
            break;
        case IN:
            run();
            break;
        case UNKNOWN:
            fprintf(stderr, "Unknown operator");
            return 1;
    }
}

int run(const char *cmd, const char *file, int flags) {

}
