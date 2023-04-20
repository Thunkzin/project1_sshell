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

        /*
        if(<, >, is in args[somewhere]){
                redirection(args)
        }
        
       void redirection(char **args, int 1st_program, int 2nd_file){
                output = getoutput(arg[1st_program]);

                return input;
       }

        */





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

        #define SIGN_TO_BE_PARSED " >|"
        /*int parsing_sign = 0;*/
        char *token;
        char **args = malloc(ARGUMENT_MAX);
        int position = 0;
        strcpy(cmd_copy, cmd);

        /*
        char *redirect_to_left;
        char *redirect_to_right;
        char *pipe;
        */

        /*
        if(strchr(cmd_copy, '<')) != NULL){
                redirection()
        }
        then there's no < in command
        */


        /*
        if((strchr(cmd_copy, '<') != NULL)){
                token = strtok(cmd_copy, "<");
                printf("there's a < \n");

        }else if ((strchr(cmd_copy, '|') != NULL)){
                token = strtok(cmd_copy, "|");
                printf("cmd_copy:%s \n", cmd_copy);
                printf("there's a | \n");
        }
        */
       /* Get the first Token , program, args[0]*/
        token = strtok(cmd_copy, " ");

        while(token != NULL) {
                // Store the rest of the tokens into args
                args[position] = token;
                position += 1;



                token = strtok(NULL, SIGN_TO_BE_PARSED);
        }
        if(position >= ARGUMENT_MAX){
                fprintf(stderr,"Error: too many process arguments\n");
                args[0] = NULL;
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
                /*
                while(!(strchr(cmd, '>') == NULL)){
                        char *mod_cmd[] = (char*)malloc(CMDLINE_MAX);
                        for (int original_string_count = 0, new_string_count = 0 ; original_string_count < strlen(cmd); original_string_count++, new_string_count++){
                                if (cmd[original_string_count] == ">") {
                                        cmd[original_string_count] = "to_right";
                                        new_string_count += 7;
                                }
                                else cmd[original_string_count] = cmd[new_string_count];
                        }
                }
                */

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


// comand1 | command2 | command3 
// comand1 | command2 
// 
/*

args = { (part1), (part2) ....}
>> $ part1 part2 
part1 = args[0]
part2 = args[1]

> <
*/
