#include "command.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

char *builtin_cmd[] = {
    "prompt",
    "pwd",
    "cd",
    "exit"
};

int getBuiltinCount() {
    return sizeof(builtin_cmd) / sizeof(char *);
}

///change prompt
int prompt_builtin(Command *cmd)
{
    extern char *prompt;
    strcpy(prompt, cmd->argv[1]);;

    return 0;
}

int pwd_builtin(Command *cmd)
{
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
               printf("%s\n", cwd);
    } else {
       perror("pwd err");
       return 1;
    }
    return 0;
}

int cd_builtin(Command *cmd)
{
    if (cmd->argv[1] == NULL) {
        if (chdir(getenv("HOME")) != 0) {
            perror("cd directory err");
        }
    }
    else
    {
        if (chdir(cmd->argv[1]) != 0) { //change directory
            perror("cd directory err");
        }
    }
    return 0;
}

int exit_builtin(Command *cmd)
{
    return 1;
}

//builtin pointer array
int (*exec_builtin[]) (Command *cmd) = {
    &prompt_builtin,
    &pwd_builtin,
    &cd_builtin,
    &exit_builtin
};
