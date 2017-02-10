#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define INPUT_BUF_SIZE 1024
#define ALLOC_ERR_MSG "Allocation Error\n"

char *getUserInput(char *line);
//char **getArgsFromInput(char *line);
int hasNotEnded(char *line, int bufSize);
void checkBufferErrorPrintAndExit();

int main(int argc, char *args[]) {
    char *line = NULL;
    //char **args = NULL; 
    
    while (1) {
        printf(">");
        line = getUserInput(line);
        printf(line);
        //args = getArgsFromInput(line);
        //execute(args);

        free(line);
        //free(args);

        line = NULL;
        //args = NULL;
    }
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
    fprintf(stderr, ALLOC_ERR_MSG);
    exit(EXIT_FAILURE);
}
