#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/wait.h>

#define MAX 10
#define BUFFER 1024

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

    for (int i = 0; i < argc - 1; i++) {
        int status;
        pid_t done = waitpid(-1, &status, 0);

        int i = -1;
        for (int j = 0; j < argc - 1; j++) {
            if (pids[j] == done) { i = j; break; }
        }

        char buffer[BUFFER] = {0};
        read(pipes[i][0], buffer, sizeof(buffer));
        printf("[%d] %s\n", i, buffer);
        close(pipes[i][0]);
    }
}
