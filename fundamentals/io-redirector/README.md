fork() duplicates the process; use the returned PID to differentiate between parent and child code. Open a file descriptor, then use dup2(oldfd, newfd) to make stdin or stdout fds point to the file. The target std file descriptor is selected with a bitwise AND and ternary operator. Close the old fd, then call execl to replace the process with the new command; file descriptors are preserved across execl. Pass the full path to the binary, its name by convention as argv[0], then any flags and arguments. Terminate the argument list with NULL.

heredoc is not covered. Only output append and trunc redirections, and input redirections

