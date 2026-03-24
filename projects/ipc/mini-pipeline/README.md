# Mini pipeline

## What it does

A minimal demonstration of Unix process piping in C, showing how two commandscan be connected via a pipe by redirecting file descriptors with dup2

## Build & run

```
$ gcc main.c -o main
$ ./main "<CMD1>" "<CMD2>"
```

## How it works

The parent process takes two command arguments and opens two pipe file descriptors. It then forks two processes for each command redirecting stdout and stdin to point to the pipe file descriptors with dup2, each process closes unnecessary file descriptors and the child processes execute their respective command. Remember every process inherits the same file desciptors as their parents and all need to be individually closed. The parent closes both pipe ends after forking and waits for both children to finish.
