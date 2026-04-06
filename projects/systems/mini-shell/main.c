/*
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

*/
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#define BUFFER_SIZE 1024
#define STRTOK_BUF 64
#define STRTOK_DELIM " \t\r\n"

void interpret(void);
char *readline(void);


int main(int argc, char *argv[]) {
    interpret();
}

void interpret(void) {
    //read, parse (tokenize and arguments), execute
    char *line;
    char **args;
    while (running) {
        printf("> ");
        line = readline();
        args = tokenize(line);
    }
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
            realloc(buffer, buffer_size);
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
    char **tokens = malloc(BUFFER_SIZE);
    if (!tokents) {
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
