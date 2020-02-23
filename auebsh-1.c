#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "p3130220-p3160120-p3160141-auebsh.h"

int main(int argc, char **argv)
{
	char *line;
	char **args;
	int status;
	do {
		printf("auebsh1> ");
		line = auebsh_read_line();
		args = auebsh_split_line(line);
		status = auebsh1_exec(args);
		free(line);
		free(args);
	} while (status);
	return EXIT_SUCCESS;
}
