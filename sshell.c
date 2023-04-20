#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

#define CMDLINE_MAX 512
#define ARGUMENT_MAX 16
#define TOKEN_LENGTH_MAX 32
#define PIPE_NUMBER_MAX 3


int system_sshell(char **args){
        pid_t pid;
        pid = fork();
        if(pid == 0){
                /* child */
                if(execvp(args[0], args) != 0){
                        perror("execvp");
                        fprintf(stderr,"Error: command not found\n");
                        return 1;
                }
        }else if(pid > 0){
                /* parent */
                int status;
                waitpid(pid, &status, 0);
        }
        return 0;
}

char** parsing_command_to_argument(char cmd[CMDLINE_MAX], char cmd_copy[CMDLINE_MAX]){
        #define SIGN_TO_BE_PARSED " \0"
        char *token;
        char **args = malloc(ARGUMENT_MAX);
        int position = 0;
        strcpy(cmd_copy, cmd);
        cmd_copy[strlen(cmd_copy)-1] = '\0';

       /* Get the first Token , program, args[0]*/
        token = strtok(cmd_copy, SIGN_TO_BE_PARSED);

        while(token != NULL) {
                // Store the rest of the tokens into args[]
                args[position] = token;
                position += 1;
                token = strtok(NULL, SIGN_TO_BE_PARSED);
        }
        if(position >= ARGUMENT_MAX){
                fprintf(stderr,"Error: too many process arguments\n");
                /* Set args[0] to be NULL so the shell will start a new loop */
                args[0] = NULL;
        }
        return(args);
}
/*
char** parsing_command_to_argument_with_redirection(char **args){
        char **args_before_redirection = malloc(ARGUMENT_MAX);
        int arg_order = 0;
        while(arg_order <= position_white_space){
                if(strchr(arg[arg_order],'<' != NULL)){
                        for(int i = 0 ; i < arg_order ; i++)
                        args_before_redirection[i] = args[i];
                }
                
        }
        return args_before_redirection;
}
*/
int main(void){
        char cmd[CMDLINE_MAX];
        char cmd_copy[CMDLINE_MAX];
        char **args;
        //char **first_args_redirection_raw;
        //char **first_args_redirection;

        
        while (1) {
                char *nl;
                int retval;

               

                /* Print prompt */
                printf("sshell@ucd$ ");
                fflush(stdout);
                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                
                /* Remove the last \0 */
                // cmd[strlen(cmd)-1] = '\0';
                // cmd[strlen(cmd)-1] = '\0';
                /* 
                if(strchr(cmd,'<') != NULL){
                        // cmd contains <
                        first_args_redirection_raw = parsing_command_to_argument(cmd, cmd_copy, '<')
                        first_args_redirection = parsing_command_to_argument(args[0], cmd_copy, ' ');
                        args = first_args_redirection;
                }
                
                */

                /* Parse the cmd into **args[] */
                args = parsing_command_to_argument(cmd,cmd_copy);

                // printf("args[0]: %s\n",args[0]);
                // printf("args[1]: %s\n",args[1]);
                // printf("args[2]: %s\n",args[2]);
                // printf("args[3]: %s\n",args[3]);
                // printf("args[4]: %s\n",args[4]);
                // printf("args[5]: %s\n",args[5]);
                // printf("args[6]: %s\n",args[6]);


                /* Redo the loop if no input */
                if (!strcmp(args[0], "\n")){
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
                        fprintf(stderr, "+ completed '%s' [0]\n", cmd);
                        break;
                }
                if (!strcmp(args[0], "pwd")) {
                        char cwd[CMDLINE_MAX];
                        getcwd(cwd, sizeof(cwd));
                        fprintf(stdout, "%s\n",cwd);
                        fprintf(stderr, "+ completed '%s' [0]\n", cmd);
                        continue;
                }
                if (!strcmp(args[0], "cd")) {
                        DIR* directory = opendir(args[1]);
                        if (directory) {
                                /* directory does exist */
                                closedir(directory);
                                chdir(args[1]);
                                fprintf(stderr, "+ completed '%s' [0]\n", cmd);
                        } else{
                                fprintf(stderr,"Error: cannot cd into directory\n");
                                fprintf(stderr, "+ completed '%s' [1]\n", cmd);
                        }
                        continue;
                }
                /* Regular command */
                retval = system_sshell(args);
                fprintf(stderr, "+ completed '%s' [%d]\n", cmd, retval);   
        }
        return EXIT_SUCCESS;
}


