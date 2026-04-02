/*
initialize (config steps) PATH?,
interpret (read from stdin and interpret): loop of shell
terminate (execute any shutdown commands, free memory...)

loop of shell: read, parse, execute

read:
use getchar to save stdin to heap buffer, every iteration check if next iteration excees buffer, if, then reallocate more space
parse:
tokenize the string by using white space as delimiter into different arguments, realloc if necessary. strtok, tokens array of pointers, token array of chars (token) in tokens
execute:
have declared built ins. check if token is built in or program. start processes, fork, exec, waipid... IPC. If built in, execute

challenges: implement piping and redirection, globbing, and quoting

*/
#include <stdio.h>

int main(int argc, char *argv[]) {

}
