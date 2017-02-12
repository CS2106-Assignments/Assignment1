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

int main() {
    char *line = NULL;
    char **args = NULL;

    while (1) {
        printf("> ");
        line = getUserInput(line);
        args = getArgsFromInput(line);
        execute(args);

        free(line);
        free(args);

        line = NULL;
        args = NULL;
    }
    return 0;
}

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
            printf("\n");
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    } else {
        perror(SHELL_ERR_MSG);
        exit(EXIT_FAILURE);
    }
}

int launchProgram(char **args) {
    execvp(args[0], args);    
    perror(SHELL_ERR_MSG);
    exit(EXIT_FAILURE);
}

int hasNotEnded(char *line, int bufSize) {
    int position = 0;
    while(1) {
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

void checkBufferErrorPrintAndExit(char *buf) {
    if (buf) {
        return;
    }
    perror(ALLOC_ERR_MSG);
    exit(EXIT_FAILURE);
}
