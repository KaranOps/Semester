#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

int main() {
    for(int i=1;i>0;i++) {
        int status;
        char str[200];
        char *args[11];  
        printf("Enter a command: ");
        fgets(str, sizeof(str), stdin);

        str[strcspn(str, "\n")] = '\0';

        char *t = strtok(str, " ");
        int i = 0;

        while (t != NULL && i < 11) {
            args[i] = t;
            t = strtok(NULL, " ");
            i++;
        }

        args[i] = NULL;  

        pid_t temp = fork();
        if (temp < 0) {
            printf("Fork failed\n");
            exit(1);
        } else if (temp == 0) {
            printf("Child process started\n");
            execv(args[0], args);  
            printf("Exec failed\n");
            exit(1);
        } else {
            wait(&status);
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                if (exit_status == 0) {
                    printf("Executed properly\n");
                } else {
                    printf("Execution failed\n");
                }
            }
        }
    }
    return 0;
}