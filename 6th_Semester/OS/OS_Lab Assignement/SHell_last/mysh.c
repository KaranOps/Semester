#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LEN 1024
#define MAX_ARGS 100

// Function to run internal commands
int run_internal(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] != NULL) chdir(args[1]);
        else printf("cd: missing argument\n");
        return 1;
    } else if (strcmp(args[0], "pwd") == 0) {
        char path[1024];
        getcwd(path, sizeof(path));
        printf("%s\n", path);
        return 1;
    } else if (strcmp(args[0], "clear") == 0) {
        printf("\033[H\033[J"); // Clear screen
        return 1;
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0); // Exit shell
    }
    return 0; // Not an internal command
}

// Function to run external commands
void run_external(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args); // Replace child with command
        printf("Command not found: %s\n", args[0]);
        exit(1);
    } else {
        wait(NULL); // Parent waits
    }
}

// Function to run a single command
void run_command(char *cmd) {
    char *args[MAX_ARGS];
    int i = 0;

    // Tokenize the command into arguments
    char *token = strtok(cmd, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    if (args[0] == NULL) return;

    if (!run_internal(args)) {
        run_external(args);
    }
}

int main(int argc, char *argv[]) {
    char line[MAX_LEN];
    FILE *input = stdin;

    // Check for batch mode
    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            printf("Cannot open file.\n");
            return 1;
        }
    }

    while (1) {
        if (input == stdin) {
            printf("mysh> ");
        }

        if (fgets(line, sizeof(line), input) == NULL) break;

        // Remove newline
        line[strcspn(line, "\n")] = '\0';

        // Split using ';'
        char *cmd = strtok(line, ";");
        while (cmd != NULL) {
            run_command(cmd);
            cmd = strtok(NULL, ";");
        }
    }

    if (input != stdin) fclose(input);
    return 0;
}
