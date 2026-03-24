# I/O Redirector

## What it does

This program displays how I/O redirection fundamentally works in C by mimicking basic shell redirection operators

- \> overwrite output redirection
- \>\> append output redirection
- \< input redirection

Heredoc << is not covered

It uses core Unix system calls: fork, open, dup2, and execl

## Build & run

```
$ gcc main.c -o main
$ ./main "<CMD>" out|out_a|in <FILE>
```

## How it works

fork() duplicates the process; use the returned PID to differentiate between parent and child code. Open a file descriptor, then use dup2(oldfd, newfd) to make stdin or stdout fds point to the file. The target std file descriptor is selected with a bitwise AND and ternary operator. Close the old fd, then call execl to replace the process with the new command; file descriptors are preserved across execl. Pass the full path to the binary, its name by convention as argv[0], then any flags and arguments. Terminate the argument list with NULL.

