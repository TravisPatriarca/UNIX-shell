extern char *builtin_cmd[];

int getBuiltinCount();

extern int (*exec_builtin[]) (Command *cmd);
