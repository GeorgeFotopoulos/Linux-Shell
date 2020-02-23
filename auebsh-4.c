#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "p3130220-p3160120-p3160141-auebsh.h"

int main(int argc, char **argv)
{	
	int fatherpid = getppid(), counter = 0;
	char *line;
	char **args;
	int status;
	do {
		counter = 0;
		if(fatherpid == getppid()) printf("auebsh4> ");
		line = auebsh_read_line();
		args = auebsh_split_line(line);
		for(int i = 0; args[i] != '\0'; i++){
			if(strcmp(args[i], "|") == 0) counter++;
			if(counter > 1) break;
		}
		if(counter > 1) {
			status = 1;
			printf("auebsh4: Cannot perform multiple pipes in this shell!\n");
		} else {
			status = auebsh4_5_exec(args);
		}
		free(line);
		free(args);
	} while (status);
	return EXIT_SUCCESS;
}
