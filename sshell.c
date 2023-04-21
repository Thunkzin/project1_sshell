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
#define MAX_ENVIRONMENT_NUMBER 25

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
char** parsing_command_to_argument(char cmd[CMDLINE_MAX],char cmd_copy[CMDLINE_MAX] , char to_be_parsed[]){
        #define SIGN_TO_BE_PARSED " \0"
        char *token;
        char **token_array = malloc(ARGUMENT_MAX);
        int position = 0;
        strcpy(cmd_copy, cmd);
        cmd_copy[strlen(cmd_copy)-1] = '\0';

       /* Get the first Token , program, args[0]*/
        token = strtok(cmd_copy, to_be_parsed);
        
        while(token != NULL) {
                // Store the rest of the tokens into args[]
                token_array[position] = token;
                position += 1;
                token = strtok(NULL, to_be_parsed);
        }
        if(position >= ARGUMENT_MAX){
                fprintf(stderr,"Error: too many process arguments\n");
                /* Set args[0] to be NULL so the shell will start a new loop */
                token_array[0] = NULL;
        }
        return(token_array);
}

int redirection(char **file) {
        int fd = open(*file, O_WRONLY);
        if (dup2(fd, STDOUT_FILENO) == -1) {
                /* Error*/
                close(fd);
                return -1;
        }
        close(fd);
        return 0;
}

void pipeline(char *cmd) {
    char *commands = &cmd[CMDLINE_MAX];
    int num_pipes = strlen(commands) - 1;
    int pipe_fds[PIPE_NUMBER_MAX][2];
    pid_t child_pids[CMDLINE_MAX];

    // Create the pipes
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("pipe");
            fprintf(stderr,"Error: no command found\n");
            exit(EXIT_FAILURE);
        }
    }

    // Fork the child processes
    for (int i = 0; i <= num_pipes; i++) {
        if ((child_pids[i] = fork()) == -1) {
            perror("fork");
            fprintf(stderr, "Error: missing command\n");
            exit(EXIT_FAILURE);
        }

        if (child_pids[i] == 0) { // Child process
            // Connect to the previous pipe (if there is one)
            if (i > 0) {
                if (dup2(pipe_fds[i-1][0], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(pipe_fds[i-1][0]);
            }

            // Connect to the next pipe (if there is one)
            if (i < num_pipes) {
                if (dup2(pipe_fds[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(pipe_fds[i][1]);
            }

            // Parse the command and execute it
            char *args[256];
            char *token;
            int j = 0;
            token = strtok(&commands[i], " \t\n");
            while (token != NULL) {
                args[j] = token;
                j++;
                token = strtok(NULL, " \t\n");
            }
            args[j] = NULL;
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // Close all the pipes
    for (int i = 0; i < num_pipes; i++) {
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
    }

    // Wait for all the child processes to terminate
    int status;
    for (int i = 0; i <= num_pipes; i++) {
        waitpid(child_pids[i], &status, 0);
        if (WIFEXITED(status)) {
            printf("[%d]", WEXITSTATUS(status));
        }
    }
    printf("\n");
}


int main(void){
        char cmd[CMDLINE_MAX];
        char cmd_copy[CMDLINE_MAX];
        char **args = malloc(ARGUMENT_MAX);
        char **alphabet_set = malloc(MAX_ENVIRONMENT_NUMBER);
        char **stored_data_set = malloc(MAX_ENVIRONMENT_NUMBER);
        int key = 0;
        // char **left_args = malloc(ARGUMENT_MAX);
        while (1) {
                char *nl;
                int retval;
                /* Print prompt */
                printf("sshell@ucd$ ");
                fflush(stdout);
                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                if(strchr(cmd, '>') != NULL){
                        args = parsing_command_to_argument(cmd, cmd_copy, ">");
                        if(args[1] == NULL){
                                fprintf(stderr,"Error: no output file\n");
                                continue;
                        }
                        if(!opendir(args[1])){
                                fprintf(stderr,"Error: cannot open output file\n");
                                continue;
                        }
                        // printf("leftargs[0]: %s\n",left_args[0]);
                        // printf("leftargs[1]: %s\n",left_args[1]);
                }else{
                        args = parsing_command_to_argument(cmd, cmd_copy, " ");
                }

                /* Parse the cmd into **args[] */
                // printf("args[0]: %s\n",args[0]);
                // printf("args[1]: %s\n",args[1]);
                // printf("args[2]: %s\n",args[2]);
                // printf("args[3]: %s\n",args[3]);
                // printf("args[4]: %s\n",args[4]);
                // printf("args[5]: %s\n",args[5]);
                // printf("args[6]: %s\n",args[6]);


                /* Redo the loop if no input */
                if (args[0] ==NULL){
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

               if (!strcmp(args[0], "set")) {
                        /* Store the environment variables and its data into pointers */
                        alphabet_set[key] = args[1];
                        stored_data_set[key] = args[2];
                        key++;
                        continue;
                }                
                if(strchr(cmd, '$') != NULL){
                        args = parsing_command_to_argument(cmd, cmd_copy, " $");
                        /* Use double loop to check if the $alphabet in the input is stored in alphabet_set */
                        for(int count = 0 ; count <= ARGUMENT_MAX; count ++){
                                for(int alphabet_count = 0 ; alphabet_count <= 25 ; alphabet_count ++){
                                        if (!strcmp(args[count], alphabet_set[alphabet_count])) {
                                                args[count] = stored_data_set[alphabet_count];
                                        }               
                                }
                        }
                }
                printf("args[0]: %s\n",args[0]);
                printf("args[1]: %s\n",args[1]);
                printf("args[2]: %s\n",args[2]);
                printf("args[3]: %s\n",args[3]);
                printf("args[4]: %s\n",args[4]);
                printf("args[5]: %s\n",args[5]);
                printf("args[6]: %s\n",args[6]);

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

        }

        return EXIT_SUCCESS;
}
