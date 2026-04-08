/*
 heavy inspiration from Stephen Brennan's Write a Shell in C

initialize (config steps) PATH?,
interpret (read from stdin and interpret): loop of shell
terminate (execute any shutdown commands, free memory...)

loop of interpret: read, parse, execute

read:
use getchar to save stdin to heap buffer, every iteration check if next iteration excees buffer, if, then reallocate more space
parse:
tokenize the string by using white space as delimiter into different arguments, realloc if necessary. strtok, tokens array of pointers, token array of chars (token) in tokens
execute:
have declared built ins. check if token is built in or program. start processes, fork, exec, waipid... IPC. If built in, execute

challenges: implement piping and redirection, globbing, and quoting

PATH is inherited from parent login shell, which is downstream of sh script /etc/profile
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define STRTOK_BUF 64
#define STRTOK_DELIM " \t\r\n"

void interpret(void);
char *readline(void);
char **tokenize(char *line);
int bin_exec(char **args);
int execute(char **args);
int builtin_cd(char **args)
int builtin_help(char **args)
int builtin_exit(char **args)

//array of commands
char *builtin[] = {
    "cd",
    "help",
    "exit"
};

//array of function pointers that return int and take pointer to char pointers (args/tokens)
int (*builtin_func[])(char **) = {
    &builtin_cd,
    &builtin_help,
    &builtin_exit
};

int main(int argc, char *argv[]) {
    interpret();
}

void interpret(void) {
    //read, parse (tokenize and arguments), execute
    do {
        printf("> ");
        char *line = readline();
        char **args = tokenize(line);
        int status = execute(args);

        free(line);
        free(args);
    } while (status);
}

char *readline(void) {
    int buffer_size = BUFFER_SIZE;
    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("malloc");
        exit(1);
    }
    int position = 0;
    int c;

    //read stdin in getc loop until EOF or new line. If next position exceeds buffer, then realloc.

    for (;;) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position++] = c;
        }

        if (position >= buffer_size) {
            buffer_size += BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                perror("realloc");
                exit(1);
            }
        }
    }
}

//strtok returns pointer to token one at a time. To use strtok subsequent times on single char string, use NULL as arg. When no more args left, returns NULL
char **tokenize(char *line) {
    int buffer_size = STRTOK_BUF;
    int position = 0;
    char **tokens = malloc(STRTOK_BUF);
    if (!tokens) {
        perror("Malloc");
        exit(1);
    }

    char *token = strtok(line, STRTOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;
        if (position >= buffer_size) {
            buffer_size += STRTOK_BUF;
            tokens = realloc(tokens, buffer_size);
            if (!tokens) {
                perror("realloc");
                exit(1);
            }
        }
        token = strtok(NULL, STRTOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

//fork and exec
int bin_exec(char **args) {
    int status;
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) perror("execvp");
        exit(1);
    } else if (pid < 0) {
        perror("fork");
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        //while status doesnt return true on exiting normally and doesnt return true on signal termination
    }
    return 1;
}

int execute(char **args) {

}

int builtin_cd(char **args) {

}

int builtin_help(char **args) {

}

int builtin_exit(char **args) {

}
