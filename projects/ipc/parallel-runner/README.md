# Parallel Runner

Runs 10 shell commands in parallel and prints their output in completion
order, fastest commands print first, regardless of argument order.

## Build & run

```
$ gcc main.c -o main
$ ./main "[CMD1]" "[CMD2]" "[CMD3]" ...
```

## How it works

For each command, the parent opens a pipe and forks a child. The child redirects its stdout to the pipe's write end and execs the command via sh -c. The parent closes the write end of each pipe after forking, it only reads. Then parent calls waitpid() to wait for whichever child finishes next, identifies it by scanning the pids array, reads its pipe, and prints the output tagged with its index. This repeats until all children are done.
