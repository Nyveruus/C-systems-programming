# Mini shell
This is a learning project to understand how shells work based on Stephen Brennan's tutorial, 
extended with signal handling and pwd.

## How Shells Work

All shells have the high level steps of interpretation and execution. Furthermore, most industry shells implement a configuration step before interpretation.
Interpretation itself can be further divided into a reading step, tokenization and/or processing step and finally execution that changes depending on if it's a built in or binary command.

The reading step involves first printing a prompt, e.g. "> " and reading from stdin to a buffer, in this case using getchar(). It is important to allocate memory correctly and safely, in our case, the position of the next iteration is always tracked through a postfix increment operator and if the next position exceeds the current buffer, it uses realloc to make the buffer larger to avoid memory issues, the buffer can be capped to prevent someone from continuously allocating undue memory. Further processing of the buffer is done through something called "tokenization" which basically returns an array of character arrays delimited by white space in the case of this project. Real shells implement quoting, a single quoted statement would always count as a single token. Tokenization can be done through the function strtok().

All processes in Unix are ultimately done through forking the parent processes and the child replacing itself with an exec function before returning its output to the parent, and all processes are technically traced back to pid 1, init, systemd... through a chain of forking and exec. The only exception to this chaing forking and exec is the init system itself which is executed by the kernel in startup. 

The next and final basic step is execution. The first thing to do is to check if the command is a built in like cd, help, pwd, exit... or if it's seperate binary. strcmp() is used for checking and returns a the result of a dereferenced and indexed function pointer if it's a built in, otherwise a seperate function for executing binaries is called with the tokens as an argument. The reason we need to differentiate between built ins and independent binaries is because for certain taks like cd or exit, the parent itself needs to be modified otherwise the child would spawn, execute, and then do nothing to actually change the parent

-

For implementing features like piping and features like IO redirection, further processing must be done on the tokens. Some design choices would be to use structs or alternatively an array of arrays of char arrays (char ***) to differentiate "segments", segments would be delimited by pipes. In the case of using a struct, in, out and the append option can be tracked for IO redirection for each segment, piping can be handled during the execution phase, by checking the number of "segments" and creating a pipe between them.

