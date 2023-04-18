#Generate executable 
sshell:	sshell.o
	gcc -Wall -Wextra -Werror -o sshell sshell.o

#Generate objects files fromm C files
sshell: sshell.c
	gcc -Wall -Wextra -Werror -c -o sshell.o sshell.c

#Clean generated files
clean:
	rm -f sshell sshell.o
