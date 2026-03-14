#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/wait.h>

int run(const char *cmd1, const char *cmd2);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./main cmd1 cmd2\n");
        return 1;
    }
    const char *cmd1 = argv[1];
    const char *cmd2 = argv[2];

    run(cmd1, cmd2);
}

int run(const char *cmd1, const char *cmd2) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execl("/bin/sh", "sh", "-c", cmd1, NULL);
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);
        execl("/bin/sh", "sh", "-c", cmd2, NULL);
        exit(1);
    }
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid, NULL, 0);
    waitpid(pid2, NULL, 0);
    return 0;
}
