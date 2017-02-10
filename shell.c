#include <stdio.h>
#include <stdlib.h>
#define MAX_BUFFER_SIZE 1024
#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIM " \t\r\n\a"
#define ALLOCATION_ERR_MSG "Allocation Error\n"

int handleInput(char *input[]);
void runMainLoop(void);
char *readLine(void);
char **splitLine(char *line);
int execute(char **args);
int launch(char **args);

/*
 * Function Declarations for built in shell commands
 */
int cd(char **args);
int help(char **args);
int quit(char **args);

/*
 * List of builtin command
 */
char *builtInStr[] = {
	"cd",
	"help",
	"exit"
};

int (*builtInFunc[]) (char**) = {
	&cd,
	&help,
	&quit
};

int getNumberBuiltIn() {
	return sizeof(builtInStr) / sizeof(char *);
}

int main(int argc, char *args[]) {
	runMainLoop();
	return 0;
}

int handleInput(char *input[]) {
	return 0;
}

void runMainLoop(void) {
	char *line;
	char **args;
	int status = 0;
	while(1) {
		printf(">");
		line = readLine();
		args = splitLine(line);
		status = execute(args);

		free(line);
		free(args);
		if (status == 0) {
			break;
		}
	}
}

char *readLine(void) {
	char *line = NULL;
	ssize_t bufSize = 0;
	getline(&line,&bufSize, stdin);
	return line;
}

char **splitLine(char *line) {
	int bufSize = TOKEN_BUFFER_SIZE, position = 0;
	char **tokens = malloc(bufSize * sizeof(char*));
	char *token;

	if (tokens == NULL) {
		fprintf(stderr, ALLOCATION_ERR_MSG);
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOKEN_DELIM);
	while (token != NULL) {
		tokens[position++] = token;

		if (position >= bufSize) {
			bufSize += TOKEN_BUFFER_SIZE;
			tokens = realloc(tokens, bufSize * sizeof(char*));
			if (tokens == NULL) {
				fprintf(stderr, ALLOCATION_ERR_MSG);
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, TOKEN_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

int launch(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		// Child Process
		if (execvp(args[0], args) == -1) {
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0 ) {
		//Error forking
		perror("lsh");
	} else {
		// Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

int execute(char **args) {
	if (args[0] == NULL) {
		return 1;
	}
	for (int i = 0; i < getNumberBuiltIn(); i++) {
		if (strcmp(args[0],builtInStr[i]) == 0) {
			return (*builtInFunc[i])(args);
		}
	}
	return launch(args);
}

int cd(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "expected args to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("lsh");
		}
	}
	return 1;
}

int help(char **args) {
	printf("Generic help screen\n");
	printf("Use the man ommand for information on other programs.\n");
	return 1;
}

int quit(char **args) {
	return 0;
}
