#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define CMDLINE_MAX 512
#define ARGUMENT_MAX 16
#define TOKEN_LENGTH_MAX 32
#define PIPE_NUMBER_MAX 3

int main(void)
{
        char cmd[CMDLINE_MAX];
        /* char args[TOKEN_LENGTH_MAX];  */

        while (1) {
                char *nl;
                int retval;

                /* Print prompt */
                printf("sshell@ucd$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmd, '\n');
                if (nl)
                        *nl = '\0';

                /* Builtin command */
                //exit
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        printf("+ completed %s [0]\n", cmd);
                        break;
                }
                //pwd
                if (!strcmp(cmd, "pwd")) {
                        char cwd[CMDLINE_MAX];
                        getcwd(cwd, sizeof(cwd));
                        printf("%s\n",cwd);
                        printf("+ completed %s [0]\n", cmd);
                        continue;
                }
                //cd
                if (!strcmp(cmd, "cd")) {
                        printf("+ completed %s [0]\n", cmd);
                        continue;
                }
                /* Regular command */
                // Parsing the commend before 
                retval = system(cmd);
                printf("+ completed %s [%d]\n", cmd, retval);
                fprintf(stderr, "Return status value for '%s': %d\n", cmd, retval);     
                        
        }
        return EXIT_SUCCESS;
}

int system_sshell(char *cmd, char *args[]){
        pid_t pid = fork();
        if(pid == 0){
                execvp(cmd, args);
                perror("execvp");
                exit(1);
        }else if(pid > 0){
                int status;
                waitpid(pid, &status, 0);
                printf("+ completed %s [%d]\n", cmd, status);

        }else{
                perror("fork");
                exit(1);
        }
        return 0;
}

char parsing_command(char *cmd){
        const char s[2] = " ";
        char *token;
        /* get the first token */
        token = strtok(cmd, s);
        /* walk through other tokens */
        while( token != NULL ) {
                printf( " %s\n", token );
                token = strtok(NULL, s);
        }
        return(0);
}

/*

char reading_command(char cmd){

}

void redirection(){

}

void pipe(){
       // program1 > file && file > program2
}

// 1 Parsing command

// 
*/
