#include "token.h"
#include "command.h"
#include "builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

void killChild() {
    int count = 1;
    pid_t pid;
    int status;

    while (count) {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid < 0)
            count = 0;
    }
}

int execute_command(int input, int output, Command *cmd) {
    if (cmd->argv == NULL) {
        printf(" NO ARGS");
        return 0;
    }

    for (int i=0; i<getBuiltinCount(); i++) {
        if (strcmp(cmd->argv[0], builtin_cmd[i]) == 0) {
            return (*exec_builtin[i])(cmd);
        }
    }

    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) {
        if (cmd->stdin_file != NULL) { //redirect input
            int fd0 = open(cmd->stdin_file, O_RDONLY, 0);
            dup2(fd0, STDIN_FILENO);
            close(fd0);
        }

        if (cmd->stdout_file != NULL) { //redirect ouput
            int fd1 = open(cmd->stdout_file, O_WRONLY|O_CREAT, 0766);
            dup2(fd1, STDOUT_FILENO);
            close(fd1);
        }

        if (input != -1)
            dup2 (input, 0);

        if (output != -1)
            dup2 (output, 1);


        execvp(cmd->argv[0], cmd->argv);


        exit(1);
    } else {
        if (strcmp(cmd->sep, conSep) != 0) {
            wait(&status);
        }

        if (input != -1)
            close (input);

        if (output != -1)
            close (output);

        signal(SIGCHLD, killChild);
        return 0;
    }
}

char *prompt;

int main(void)
{
    sigset_t sigs;

    sigemptyset(&sigs);
    sigaddset(&sigs, SIGINT);
    sigaddset(&sigs, SIGQUIT);
    sigaddset(&sigs, SIGTSTP);

    sigprocmask(SIG_SETMASK, &sigs, NULL);


    if (!(prompt = malloc(256 * sizeof(char))))
        return 1;
    strcpy(prompt, "%");

    char *str;
    size_t len = 256;
    size_t i = 0;
    int status;
    do {
       printf("\033[0;33m");
       printf("%s ", prompt);
       printf("\033[0m");

       int again = 1;
       while (again) {
            again = 0;
            i = getline(&str, &len, stdin);
            str[i-1] = '\0';
            if (str == NULL)
                if (errno == EINTR)
                     again = 1;        // signal interruption, read again
       }

        char **token = calloc(1000, sizeof(char*));
        tokenise(str, token);

        Command cmds[MAX_NUM_COMMANDS];
        for (int i=0; i<MAX_NUM_COMMANDS; i++)
        {
            cmds[i].first = 0;
            cmds[i].last = 0;
            cmds[i].sep = NULL;
            cmds[i].argv = NULL;
            cmds[i].stdin_file = NULL;
            cmds[i].stdout_file = NULL;
        }

        int num = separateCommands(token, cmds);

        //get pipe count
        int pipeCount = 0;
        for (int x=0; x<num; x++) {
            if (strcmp(cmds[x].sep, pipeSep) == 0) {
                pipeCount += 1;
            }
        }

        //initialise pipes
        int pipefds[2*pipeCount];
        for(int i = 0; i < pipeCount; i++ ){
            if( pipe(pipefds + i*2) < 0 ){
            }
        }

        int m = 0;
        for (int i=0; i<num; i++) {
            int in = -1;
            int out = -1;
            if (i != 0) { //input
                if (strcmp(cmds[i-1].sep, pipeSep) == 0) {
                    in = pipefds[m]; // 0
                    m+=2;
                }
            }

            if (strcmp(cmds[i].sep, pipeSep) == 0) { //output
                out = pipefds[m+1]; // 1
            }

            status = execute_command(in, out, &cmds[i]);
        }
    } while (!status);

    return(0);
}
