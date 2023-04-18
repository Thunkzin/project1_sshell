#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define CMDLINE_MAX 512
#define ARGUMENT_MAX 16
#define TOKEN_LENGTH_MAX 32

int main(void)
{
        char cmd[CMDLINE_MAX];

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
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        break;
                }
                if (!strcmp(cmd, "cd")) {
                        fprintf(stderr, "Bye...\n");
                        break;
                }
                if (!strcmp(cmd, "pwd")) {
                        fprintf(stderr, "Bye...\n");
                        break;
                }
                /* Regular command */
                retval = system(cmd);
                printf("+ completed %s [%d]\n", cmd, retval);
                fprintf(stderr, "Return status value for '%s': %d\n",
                        cmd, retval);
                        
        }
        return EXIT_SUCCESS;
}

int system_sshell(char *cmd, char *args[]){
        pid_t pid = fork();
        if(pid == 0){
                execv(cmd, args);
                perror("execv");
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