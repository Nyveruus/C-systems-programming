//fork duplicate
//open
//dup2 makes newfd point to same as oldfd
//close
//exec replace current program with new

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

typedef enum {
    OUT,
    OUT_A,
    IN,
    UNKNOWN
} redirect_opts;

redirect_opts parse_op(const char *op) {
    if (strcmp(op, "out") == 0) return OUT;
    if (strcmp(op, "out_a") == 0) return OUT_A;
    if (strcmp(op, "in") == 0) return IN;
    return UNKNOWN;
}

int run(const char *cmd, const char *file, int flags);

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: ./main cmd out|out_a|in file");
        return 1;
    }
    const char *cmd = argv[1];
    const char *operator = argv[2];
    const char *file = argv[3];
    switch (parse_op(operator)) {
        case OUT:
            run(cmd, file, O_WRONLY | O_CREAT | O_TRUNC);
            break;
        case OUT_A:
            run(cmd, file, O_WRONLY | O_APPEND | O_CREAT);
            break;
        case IN:
            run(cmd, file, O_RDONLY);
            break;
        case UNKNOWN:
            fprintf(stderr, "Unknown operator");
            return 1;
    }
}

int run(const char *cmd, const char *file, int flags) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        int fd = open(file, flags, 0644);
        if (fd < 0) {
            perror("open");
            exit(1);
        }
        int target = (flags & O_WRONLY) ? STDOUT_FILENO : STDIN_FILENO;
        dup2(fd, target);
        close(fd);
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        exit(1);
    }
    int status;
    waitpid(pid, &status, 0);
    return 0;
}
