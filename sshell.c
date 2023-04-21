#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>

#define CMDLINE_MAX 512
#define ARGUMENT_MAX 16
#define TOKEN_LENGTH_MAX 32
#define PIPE_NUMBER_MAX 3

int redirection_stdout(char **file) {
        int fd = open(file, O_WRONLY);
        if (fd == -1) {
                /* Error */
                return -1;
        }
        if (dup2(fd, STDOUT_FILENO) == -1) {
                close(fd);
                return -1;
        }
        close(fd);
        return 0;
}

int system_sshell(char **args){
        pid_t pid;
        pid = fork();
        int status = 0;
        if(pid == 0){
                /* child */
                execvp(args[0], args);
                perror("execvp");
                fprintf(stderr,"Error: command not found\n");
                exit(1);
        }else if(pid > 0){
                /* parent */
                waitpid(pid, &status, 0);
        }
        return WEXITSTATUS(status);
}

char** parsing_command_to_argument(char cmd[CMDLINE_MAX],char cmd_copy[CMDLINE_MAX] , char to_be_parsed[2]){
        #define SIGN_TO_BE_PARSED " \0"
        char *token;
        char **args = malloc(ARGUMENT_MAX);
        int position = 0;
        strcpy(cmd_copy, cmd);
        cmd_copy[strlen(cmd_copy)-1] = '\0';

       /* Get the first Token , program, args[0]*/
        token = strtok(cmd_copy, to_be_parsed);
        
        while(token != NULL) {
                // Store the rest of the tokens into args[]
                args[position] = token;
                position += 1;
                token = strtok(NULL, to_be_parsed);
        }
        if(position >= ARGUMENT_MAX){
                fprintf(stderr,"Error: too many process arguments\n");
                /* Set args[0] to be NULL so the shell will start a new loop */
                args[0] = NULL;
        }
        return(args);
}

// void redirection(char** before_redirection, char** after_redirection){

// }

int main(void){
        char cmd[CMDLINE_MAX];
        char cmd_copy[CMDLINE_MAX];
        char **args = malloc(ARGUMENT_MAX);
        // char **redirection_args;
        //
        // char **right_args;
        // char **left_args_parsed_white_space;
        // char **right_args_parsed_white_space;
        
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
                // if(strchr(cmd, '>') != NULL){
                //         /* Command contains redirection */
                //         printf("aaaaa");
                //         redirection_args = parsing_command_to_argument(cmd, cmd_copy, ">");
                //         printf("bbbbb");
                //         right_args = *redirection_args[1];
                //         left_args_parsed_white_space = parsing_command_to_argument(redirection_args[0], cmd_copy, " ");
                //         right_args_parsed_white_space = parsing_command_to_argumen(&right_args, cmd_copy, " ");
                //         system_sshell(left_args_parsed_white_space);
                        
                // }else{
                //         /* Command withouth redirection */
                // }
                if(strchr(cmd, '>') != NULL){
                        args = parsing_command_to_argument(cmd, cmd_copy, ">");
                        char **left_args = malloc(ARGUMENT_MAX);
                        left_args = parsing_command_to_argument(args[0], cmd_copy, " ");
                        printf("leftargs[0]: %s\n",left_args[0]);
                        printf("leftargs[1]: %s\n",left_args[1]);
                        printf("leftargs[2]: %s\n",left_args[2]);
                        printf("leftargs[3]: %s\n",left_args[3]);
                        printf("....\n");
                }else{
                args = parsing_command_to_argument(cmd, cmd_copy, " ");

                }
                

                /* Parse the cmd into **args[] */
                

                printf("args[0]: %s\n",args[0]);
                printf("args[1]: %s\n",args[1]);
                printf("args[2]: %s\n",args[2]);
                printf("args[3]: %s\n",args[3]);
                printf("args[4]: %s\n",args[4]);
                printf("args[5]: %s\n",args[5]);
                printf("args[6]: %s\n",args[6]);


                /* Redo the loop if no input */
                if (!strcmp(args[0], "\n")){
                        free(args);
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
                        free(args);
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
                        free(args);
                        continue;
                }else{
                        /* Regular command */
                        retval = system_sshell(args);
                        fprintf(stderr, "+ completed '%s' [%d]\n", cmd, retval);   
                        free(args);
                }
                free(args[0]);

        }

        return EXIT_SUCCESS;
}
