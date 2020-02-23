#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int auebsh_cd(char **args);

int auebsh_help(char **args);

int auebsh_exit(char **args);

int auebsh_launch(char **args);

int auebsh_execute(char **args);

char *auebsh_read_line(void);

char **auebsh_split_line(char *line);

int auebsh1_exec(char **args);

int auebsh2_exec(char **args);

int auebsh3_exec(char **args);

int auebsh4_5_exec(char **args);
