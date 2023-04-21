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

### Parsing commands

The `parsing_command_to_argument()` function is responsible for parsing a command 
entered by the user into individual arguments, which are then stored in an array 
called args using the `strtok()` function. The purpose of this function is to separate 
the program name (the first argument) from the arguments passed to the program (the 
subsequent arguments), and to store them in separate elements of the args array. The 
function also checks for errors and returns a pointer to the args array.

The function takes two parameters: `cmd`, a character array containing the command 
entered by the user. `cmd_copy`, a character array used as a copy of `cmd` to avoid 
modifying the original command string.

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




### Piping




### Error management
