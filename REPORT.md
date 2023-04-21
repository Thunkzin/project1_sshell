# ECS150_Project#1_sshell: A simple shell

## Summary

The `sshell` program is a basic shell that allows users to input command-lines and 
have them executed. The program has several core features, including the ability to 
execute commands with optional arguments, redirect command output to files, and pipe 
commands together. The program accomplishes these basic I/O operations through several 
key functions.

## Implementation

The implementation of this program follows following steps:
1. Making a Makefile to generate an executable `sshell` from `sshell.c` using GCC. 
2. Handling the command lines by parsing as individual arguments.  
3. Implementations of `builtin` commands.
4. Manipulation of stdout file descriptor to `redirect` to the output file instead 
of printing it to the terminal. 
5. Pipeline function using a combination of parsing, forking, and inter-process 
communication using pipes. 
6. Error management.

### Parsing commands

The `parsing_command_to_argument()` function is responsible for parsing a command 
entered by the user into individual arguments, which are then stored in an array 
called args using the `strtok()` function. The purpose of this function is to separate 
the program name (the first argument) from the arguments passed to the program (the 
subsequent arguments), and to store them in separate elements of the args array. The 
function also checks for errors and returns a pointer to the args array.

The function takes three parameters: `cmd`, a character array containing the command 
entered by the user. `cmd_copy`, a character array used as a copy of `cmd` to avoid 
modifying the original command string. And `char to_be_parsed[]`, a character that 
we like to used as a dilemma. Therefore, despite the commandline will includes
redirection sign, white space, and pipe, we can brake up the parsing stage with 
different dilemma when we needs.

First, the function uses the `strtok()` function to tokenize the cmd_copy 
string, using whitespace as the delimiter. It then stores each token in the `arg` 
array, incrementing the position variable each time to move to the next element in the 
array.  If the number of arguments parsed exceeds the maximum number of arguments 
allowed (ARGUMENT_MAX), an error message is printed to the standard error stream and 
args[0] is set to NULL to signal to the shell to start a new loop.

### Builtin Commands

The `builtin` commands in this program are implemented directly in the main function, 
using if statements to check if the command is a built-in command. If it is, the 
appropriate action is taken, such as changing the working directory or printing the 
current directory. If the command is not a built-in command, the shell will try to 
execute it as an external program using the "system_sshell" function and print out an 
error message.

If the user entered a regular command (i.e., not a built-in command), the program used 
`system_sshell()` to execute the command with the parsed arguments. The program then 
printed the exit status of the command.

The command `exit` is used to terminate the shell program. When the user types in 
`exit` as a command, the program checks if the first argument in the args array (which 
was obtained by parsing the user's input command) is equal to the string `exit` using 
the `strcmp()` function. If it is, the program prints the message "Bye..." to stderr 
using `fprintf()`, indicating that the shell is exiting. Then it prints another 
message to stderr indicating that the command has been completed with exit code 0.

The command `pwd` displays the current working directory. This is implemented by 
calling the `getcwd()` function to get the current working directory and then printing 
it to the terminal using the `fprintf()` function. The program then prints the 
completed command with the exit status using `fprintf()` on stderr.

The command `cd` changes the current working directory to the directory specified by 
the user. This is done by calling the `chdir()` function with the directory specified 
as an argument. The program then prints the completed command with the exit status 
using `fprintf()` on stderr.

### Output redirection
The output redirection function will be called when the `cmd` includes `>` sign, which
will be detected before applying `parsing_command_to_argument()` function. Firstly, 
parse the `cmd` to two strings (left hand sides and right hand sides string) by 
calling `parsing_command_to_argument()` function with `>` being used as 
`char to_be_parsed[2]` parameter. Secondly, parsing these two strings again but using 
white space as the `char to_be_parsed[2]` parameter this time. Then, call the 
redirection function to redirect the stdout to the file's directory by `dup2()` funciton. 
Finally, run the left hand sides string by implementing `system_sshell()`, and the output 
will be directed into the file (right hand sides string).

### Piping
 
The `pipeline()`function in sshell is responsible for handling command pipelines, 
which allows multiple commands to be executed in sequence where the output of each 
command is passed as input to the next command.

The function takes in a command string as an argument, which contains multiple 
commands separated by the pipe symbol "|". It then splits the command string into 
separate commands and creates pipes to connect the input and output of each command. 
Finally, it forks child processes to execute each command and waits for all child 
processes to complete.

The first step in `pipeline()` is to parse the input command string into separate 
commands. This is done using the `strtok()` function to split the string into tokens 
using the "|" symbol as the delimiter. The resulting array of commands is then 
processed in a loop.

Forking child processes: For each command, a child process is forked using the `fork()` 
system call. The child process is responsible for executing the command. Before 
executing the command, the child process sets up the standard input and output using 
the pipe file descriptors created in the previous step. The `dup2()` system call is used 
to redirect the standard input or output of the child process to the read or write end 
of the appropriate pipe.

Once the child process has set up its standard input and output, it executes the 
command using the execvp() system call. The arguments to `evecvp()` are the command and 
its arguments, which were extracted earlier in the parsing step.

After all the child processes have been forked, the parent process closes all the pipe 
file descriptors, since they are no longer needed. The parent process also waits for 
all child processes to complete using the `waitpid()` system call. The exit status of 
each child process is checked using the `WIFEXITED()` and `WEXITSTATUS()` macros to 
determine whether the child process completed successfully or with an error.


### Error management
When it comes to the Error management, it can be roughly derived into three kinds, 
which are Failure of library functions, Errors during the parsing of the command line,
and Errors during the launching of the job. The Failure of library functions will be 
handled in the `system_sshell()`, by the perror() function right after the execvp(), 
and will be directed into stderr. 

For instance, the error that we encountered the most
during the projects is the Bad Address error, which happened occasionally in the 
mac's build-in terminal, but always happens in LINUX system's machines. 
Unfortunately, we are not able to solve this issues on times.

For the errors during the parsing of the command line, these are handled after or in 
`parsing_command_to_argument()` function, and this kinds of errors are mainly caused 
invalid user input if having correct parsing function. Therefore, among three kinds 
of error, this is the relatively easier one to deal with while doing the project.

Finally, the Launching errors are either handled in the build-in command section or
in the `system_sshell()` function, and it wasn't too difficult to deal with these
kinds of error since there're pretty much single line function that can directly 
do the job for us. For example, when we're holding the "Error: cannot cd into
directory," we've just simply used the 'opendir()' function to detected if the
directories existed.

### Debugging issues
The very first significant bugs that we encountered is to decide which lines should 
be directed out into stderr and stdout while applying fprintf() function. And the 
most time consuming bugs that we had was that when piping the command line from 
echo function to the sshell, the stdout doesn't print out new line right after the
command line is inputed. 

For example, when input the command line `echo -e "ls\nexit\n" | ./sshell`  in the
local shell, the output becomes like sshell@ucd ls"output of ls", which sticked 
behind the sshell@ls without changing line.

Finally, the bugs that we didn't solve is the Bad Address error which happened 
when entering several command with options. For instance, "mkdir -p dir_test"
will cause a bad address error, but "mkdir dir_test" wouldn't. However, when
we use date command to test out, both "date -u" and "date" works for our sshell.

