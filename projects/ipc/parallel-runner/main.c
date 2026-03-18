#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/wait.h>

#define MAX 10

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Usage: at least 1 command\n");
        return 1;
    }
    pid_t pids[MAX] = {0};
    int pipes[MAX][2] = {0};
    for (int i = 0; i < argc - 1; i++) {
        if (i >= MAX) break;
        pipe(pipes[i]);
        pids[i] = fork();
        if (pids[i] == 0) {
            dup2(pipes[i][1], STDOUT_FILENO);
            close(pipes[i][0]);
            close(pipes[i][1]);
            execl("/bin/sh", "sh", "-c", argv[i + 1], NULL);
            exit(1);
        }
        close(pipes[i][1]);
    }
}
