#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SHELL_PATH "SHELL_PATH="

char *printAllEnvVarsAndGetShellPath(char *envp[]);
void printOnlyShellPath(char *shellPath);

int main(int argc, char *argv[], char *envp[]) {
    char *shellPath = printAllEnvVarsAndGetShellPath(envp);
    printOnlyShellPath(shellPath);
    return 0;
}

/**
 * Prints all environment variables
 *
 * return shellPath string if it was found
 */
char *printAllEnvVarsAndGetShellPath(char *envp[]) {
    int count = 0;
    char *shellPath = NULL;
    while(envp[count] != NULL) {
        if (shellPath == NULL) {
            shellPath = strstr(envp[count], SHELL_PATH);
        }
        printf("%s\n", envp[count++]);
    }
    return shellPath;
}

/**
 * Prints shell path if it is found
 * Prints UNKNOWN if the shell is not found
 */
void printOnlyShellPath(char *shellPath) {
    char *shellPathToDisplay = NULL;
    if (shellPath == NULL) {
        shellPathToDisplay = "UNKNOWN";
    } else {
        int numCharToIgnore = strlen(SHELL_PATH);
        int totalStrLen = strlen(shellPath);
        int subStringLen = totalStrLen - numCharToIgnore;
        shellPathToDisplay = malloc(sizeof(char*) * subStringLen+1);
        strncpy(shellPathToDisplay, shellPath + numCharToIgnore, subStringLen);
        shellPathToDisplay[subStringLen] = '\0';
    }
    printf("\nSHELL_PATH value is %s\n", shellPathToDisplay);
}
