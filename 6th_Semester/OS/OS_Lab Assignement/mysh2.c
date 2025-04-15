#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define input_size 1024
#define max_args 100

// Function declarations 
void run_command(char *cmd);         
void run_external(char **args);
int run_internal(char **args);

int main(int argc, char *argv[])
{
    char line[input_size];
    FILE *input = stdin;

    if (argc == 2)
    {
        input = fopen(argv[1], "r");
        if (!input)
        {
            printf("Cannot open file.\n");
            return 1;
        }
    }

    while (1)
    {
        if (input == stdin)
        {
            printf("mysh> ");
        }

        if (fgets(line, sizeof(line), input) == NULL)
            break;

        line[strcspn(line, "\n")] = '\0'; //  //"strcspn" length return krta hai upto which second argument is found(remove trailing newline)

        char *cmd = strtok(line, ";");
        while (cmd != NULL)
        {
            run_command(cmd);
            cmd = strtok(NULL, ";");
        }
    }

    if (input != stdin)
        fclose(input);

    return 0;
}

void run_command(char *cmd)
{
    char *args[max_args];
    int i = 0;

    // Tokenize the command into arguments
    char *token = strtok(cmd, " ");
    while (token != NULL)
    {
        args[i++] = token;
        token = strtok(NULL, " "); // split kr deta hai string ko multiple pieces me using second argument
    }
    args[i] = NULL;

    if (args[0] == NULL)
        return;

    if (!run_internal(args))
    {
        run_external(args);
    }
}

void run_external(char **args)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(args[0], args); // child ko command se replace karengee
        printf("Command not found: %s\n", args[0]);
        exit(1);
    }
    else
    {
        wait(NULL);  // Parent waits
    }
}

int run_internal(char **args)
{
    return 0;
}