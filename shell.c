/*
 * Name: Yeo Xiang Guang Brandon
 * Matric: A0135782Y
 *
 * Name: Ryan Tan Wen Jun
 * Matric: A0135747X
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define INPUT_BUF_SIZE 1024
#define TOKEN_BUF_SIZE 64
#define ALLOC_ERR_MSG "Allocation Error\n"
#define TOKEN_DELIM " "
#define SHELL_ERR_MSG "Shell error: "

char *getUserInput(char *line);
char **getArgsFromInput(char *line);
int hasNotEnded(char *line, int bufSize);
void checkBufferErrorPrintAndExit(char *buf);
void execute(char **args);
int launchProgram(char **args);
void freeVariables(char *line, char **args);

int main() {
    char *line = NULL;
    char **args = NULL;

    while (1) {
        printf("> ");
        line = getUserInput(line);
        args = getArgsFromInput(line);
        execute(args);

        freeVariables(line, args);
    }
    return 0;
}

/**
 * Frees variables that have been allocated memory.
 * 
 * param line is a string representation of the file name.
 * param args is an array of string.
 */
void freeVariables(char *line, char **args) {
    free(line);
    free(args);

    line = NULL;
    args = NULL;
}

/**
 * Obtains user input in a line.
 * If the buffer is not big enough, increase it by 2 times and realloc memory.
 */
char *getUserInput(char *line) {
    int bufSize = INPUT_BUF_SIZE;
    line = malloc(sizeof(*line) * bufSize);
    checkBufferErrorPrintAndExit(line);

    while(fgets(line, bufSize, stdin) != NULL) {
        if (hasNotEnded(line, bufSize)) {
            bufSize *= 2;
            line = realloc(line, sizeof(*line) * bufSize);
            checkBufferErrorPrintAndExit(line);
        } else {
            break;
        }
    }
    return line;
}

/**
 * Gets arguments from the line of text.
 * Arguments are delimetered by TOKEN_DELIM which is just " ".
 * If token buffer is not big enough, increase it by 2 times and realloc memory.
 * The string array is NULL terminated.
 */
char **getArgsFromInput(char *line) {
    char **tokens = malloc(sizeof(char*) * TOKEN_BUF_SIZE);
    char *token;
    int tokenSize = TOKEN_BUF_SIZE;
    int argsCounter = 0;
    
    checkBufferErrorPrintAndExit((char*)tokens);
    token = strtok(line, TOKEN_DELIM);
    while (token != NULL) {
        tokens[argsCounter] = token;
        argsCounter += 1;

        token = strtok(NULL, TOKEN_DELIM);
        if (argsCounter >= tokenSize) {
            tokenSize *= 2;
            tokens = realloc(tokens, sizeof(char*) * tokenSize);
            checkBufferErrorPrintAndExit(line);
        }
    }
    tokens[argsCounter] = NULL;
    return tokens;
}

/**
 * Executes the command using the file name and the arguments.
 * Forks a new process to handle the executing of the command.
 * Parent will wait for child to be terminated to proceed.
 * Prints parent id in child process.
 * Prints child id in parent process.
 *
 * If error occurs, display SHELL_ERR_MSG.
 */
void execute(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) { // Child process
        printf("> Parent id: %d\n", getppid());
        launchProgram(args);
    } else if (pid > 0) { // Parent process
        printf("Loading new process with id %d\n", pid);
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        printf("\n");
    } else {
        perror(SHELL_ERR_MSG);
        exit(EXIT_FAILURE);
    }
}

/**
 * Launches the child program using the child name and other arguments.
 *
 * If error occurs, display SHELL_ERR_MSG.
 */
int launchProgram(char **args) {
    if (execvp(args[0], args) == -1) {
        perror(SHELL_ERR_MSG);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

/**
 * Util method to check if there is more lines to be read.
 * Returns true if there are more characters to read
 * Returns false if there is an EOF or end line.
 */
int hasNotEnded(char *line, int bufSize) {
    int position = 0;
    while (1) {
        if (line[position] == EOF || line[position] == '\n') {
            line[position] = '\0';
            return 0;
        }
        position += 1;
        if (position >= bufSize) {
            return 1;
        }
    }
}

/**
 * Displays error when there is an issure with allocation memory
 */
void checkBufferErrorPrintAndExit(char *buf) {
    if (buf) {
        return;
    }
    perror(ALLOC_ERR_MSG);
    exit(EXIT_FAILURE);
}
