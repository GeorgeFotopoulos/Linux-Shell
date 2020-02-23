#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "p3130220-p3160120-p3160141-auebsh.h"

char *builtin_str[] = { "cd", "help", "exit" };
int (*builtin_func[]) (char **) = { &auebsh_cd, &auebsh_help, &auebsh_exit };

int auebsh_num_builtins()
{
	return sizeof(builtin_str) / sizeof(char *);
}

int auebsh_cd(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "auebsh: expected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("auebsh");
		}
	}
	return 1;
}

int auebsh_help(char **args)
{
	int i;
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are built in:\n");
	
	for (i = 0; i < auebsh_num_builtins(); i++) {
		printf("  %s\n", builtin_str[i]);
	}
	
	printf("Use the man command for information on other programs.\n");
	return 1;
}

int auebsh_exit(char **args)
{
	printf("\n");
	return 0;
}

int auebsh_launch(char **args)
{
	pid_t pid;
	int status;
	
	pid = fork();
	if (pid == 0) {
		// Child process
		if (execvp(args[0], args) == -1) {
			perror("auebsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// Error forking
		perror("auebsh");
	} else {
		// Parent process
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	return 1;
}

int auebsh_execute(char **args)
{
	int i;
	
	if (args[0] == NULL) {
		return 1;
	}
	
	for (i = 0; i < auebsh_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}
	
	return auebsh_launch(args);
}

#define AUEBSH_RL_BUFSIZE 255
char *auebsh_read_line(void)
{
	int bufsize = AUEBSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;
	
	if (!buffer) {
		fprintf(stderr, "auebsh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	
	while (1) {
		c = getchar();
		
		if (c == EOF) {
			printf("\n");
			exit(EXIT_SUCCESS);
		} else if (c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;
		
		if (position >= bufsize) {
			fprintf(stderr, "auebsh: allocation error1\n");
			exit(EXIT_FAILURE);
		}
	}
}

#define AUEBSH_TOK_BUFSIZE 255
#define AUEBSH_TOK_DELIM " \t\r\n\a"
char **auebsh_split_line(char *line)
{
	int bufsize = AUEBSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token, **tokens_backup;
	
	if (!tokens) {
		fprintf(stderr, "auebsh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	
	token = strtok(line, AUEBSH_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;
		
		if (position >= bufsize) {
			fprintf(stderr, "auebsh: allocation error!!\n");
			exit(EXIT_FAILURE);
		}
		
		token = strtok(NULL, AUEBSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

int auebsh1_exec(char **args) {
	for(int i = 0; args[i] != '\0'; i++) {
		if(strncmp(args[i],"-", 1) == 0) {
			printf("command not found\n");
			return -1;
		}
	}
	return auebsh_execute(args);
}

int auebsh3_exec(char **args) {
	int fd0 = 0, fd1 = 0, j= -1, x= -1, in = 0, out = 0, status, pid=0, parentPid=0;
	char input[64], output[64];
	for(int i = 0; args[i] != '\0'; i++) {
		if (strcmp(args[i], "<") == 0) {
			strcpy(input, args[i+1]);
			j = i;
			in = 2;
		}
		if (strcmp(args[i], ">") == 0) {
			x = i;
			strcpy(output, args[i+1]);
			out = 2;
		}
	}
	if (j == -1 && x == -1) {
		if (pid == parentPid) {
			
			return auebsh_execute(args);
		}
	} else {
		pid = fork();
		if (pid == 0) {
			if (j != -1) args[j] = NULL;
			if (x != -1) args[x] = NULL;	
			if (in) {

				if ((fd0 = open(input, O_RDONLY)) < 0) {
					perror("Couldn't open input file");
					exit(0);
				}		
				dup2(fd0, 0);
				in = 0;
				close(fd0);
				if(!out){
					status = auebsh_execute(args);
					exit(1);
				}
			
			}
			if (out) {
				
				if ((fd1 = creat(output , 0644)) < 0) {
					perror("Couldn't open the output file");
					exit(0);
				}
				dup2(fd1, 1);
				out = 0;
				close(fd1);
				if(!in){
					status = auebsh_execute(args);
					exit(1);
				}
				
			}
			return auebsh_execute(args);
		} else if (pid > 0) {
			do {
				waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
			return 1;
		} else {
			perror("auebsh-3");
			exit(0);
		}
	}
}

int auebsh2_exec(char **args) {
	for(int i = 0; args[i] != '\0'; i++) {
		if(strncmp(args[i],"-", 1) == 0) {
			printf("command not found\n");
			return -1;
		}
	}
	return auebsh3_exec(args);
}

void docommand(char **args1,char **args2)  
{
	int pipefd[2], pipeFound;
	int bufsize = 255, temp, x;
	char **args3 = malloc(sizeof(char) * bufsize);
	char **args4 = malloc(sizeof(char) * bufsize);	
   
    if (pipe(pipefd)) {
        perror("pipe");
        exit(127);
    }

    switch (fork()) {
		case -1:
		    perror("fork");
			exit(0);
		case 0:
		    close(pipefd[0]);
		    dup2(pipefd[1], 1);
		    close(pipefd[1]);
		    auebsh3_exec(args1);
			exit(0);
		default:
			pipeFound = 0;
	   		for (int u = 0; args2[u] != '\0'; u++) {
				if (strcmp(args2[u],"|") == 0) {
					pipeFound = 1;
					temp = u;
					for (x = 0; x < temp; x++) {
						args3[x] = args2[x];
					}
					args3[x] ='\0';
					for (x = temp + 1; args2[x] != '\0'; x++) {
						args4[x - temp-1] = args2[x];
						args2[x] = NULL;
					}
				}
			}

			if(pipeFound == 0) {
				close(pipefd[1]);
				dup2(pipefd[0], 0);
				close(pipefd[0]);
				auebsh3_exec(args2);
				exit(0);
			} else {
				close(pipefd[1]);
				dup2(pipefd[0], 0);
				close(pipefd[0]);
				docommand(args3, args4);
			}
	}
}

int auebsh4_5_exec(char **args) {
	int bufsize = AUEBSH_TOK_BUFSIZE, temp = -1, x, i;
	char **args1 = malloc(sizeof(char) * bufsize);
	char **args2 = malloc(sizeof(char) * bufsize);
	
	for (int u = 0; args[u] != '\0'; u++) {
		if (strcmp(args[u],"|") == 0) {
			temp = u;
			for (x = 0; x < temp; x++) {
				args1[x] = args[x];
			}
			args1[x] ='\0';
			for (x = temp + 1; args[x] != '\0'; x++) {
				args2[x - temp-1] = args[x];
			}
			break;
		}
	}
	
	if (temp == -1 ) {	
		return auebsh3_exec(args);
	} else {
		int pid, status;
		fflush(stdout); 
		switch ((pid = fork())) {
			case -1:
				perror("fork");
				break;
			case 0:
				// child
				docommand(args1 ,args2);
				break; // not reached
			default:      
				pid = wait(&status);
		}
	}
}
