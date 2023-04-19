#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMDLINE_MAX 512
#define ARGUMENT_MAX 16
#define TOKEN_LENGTH_MAX 32
#define PIPE_NUMBER_MAX 3


int system_sshell(char **args){
        pid_t pid = fork();
        if(pid == 0){
                execvp(args[0], args);
                perror("execvp");
                exit(1);
        }else if(pid > 0){
                int status;
                waitpid(pid, &status, 0);
        }else{
                perror("fork");
                exit(1);
        }
        return 0;
}

char** parsing_command_to_argument(char cmd[CMDLINE_MAX], char cmd_copy[CMDLINE_MAX]){
        char *token;
        char **args = malloc(ARGUMENT_MAX);
        int position = 0;
        strcpy(cmd_copy, cmd);

        // get the first token
        token = strtok(cmd_copy, " ");
        while(token != NULL) {
                // Store the rest of the tokens into args
                args[position] = token;
                position += 1;
                token = strtok(NULL,  " ");
        }
        if(position >= ARGUMENT_MAX){
                fprintf(stderr,"Error: too many process arguments\n");
                return NULL;
        }
        return(args);
}

int main(void){
        char cmd[CMDLINE_MAX];
        char cmd_copy[CMDLINE_MAX];
        char **args;

        while (1) {
                char *nl;
                int retval;

                /* Print prompt */
                printf("sshell@ucd$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin); 
                /* Remove the last \0 */
                cmd[strlen(cmd)-1] = '\0';
               
                /* Test
                printf("cmd = :%s \n", cmd);
                */

                /* Parse the cmd into **args[] */
                args = parsing_command_to_argument(cmd, cmd_copy);
               
                /* Test
                printf("cmd = :%s \n", cmd);
                */

                /*
                printf for test
                printf("args[0]: %s\n",args[0]);
                printf("args[1]: %s\n",args[1]);
                printf("args[2]: %s\n",args[2]);
                */

                /* Redo the loop if the error is received */
                if (args[0]== NULL){
                        continue;
                }
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
                if (!strcmp(args[0], "exit")) {
                        fprintf(stderr, "Bye...\n");
                        printf("+ completed %s [0]\n", args[0]);
                        break;
                }
                if (!strcmp(args[0], "pwd")) {
                        char cwd[CMDLINE_MAX];
                        getcwd(cwd, sizeof(cwd));
                        printf("%s\n",cwd);
                        printf("+ completed %s [0]\n", args[0]);
                        continue;
                }
                if (!strcmp(args[0], "cd")) {
                        chdir(args[1]);
                        printf("+ completed %s [0]\n", args[0]);
                        continue;
                }
                /* Regular command */
                // Parsing the commend before 
                retval = system_sshell(args);
                printf("+ completed %s [%d]\n", cmd, retval);   
                        
        }
        return EXIT_SUCCESS;
}
