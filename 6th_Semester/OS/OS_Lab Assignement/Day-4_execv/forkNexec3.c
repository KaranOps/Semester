#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>

int main() {
    for(int i=1;i>0;i++){
        int status;
        char s[100];
        printf("Enter a string: ");
        fgets(s, sizeof(s), stdin);
       
       	s[strcspn(s, "\n")] = '\0';	
        pid_t temp = fork();
        
        if(temp < 0){
            printf("Fork not created\n");
            exit(1);
        } else if(temp == 0){
            char *args[] = {s, NULL};
            printf("Child process started\n");
            execv(s, args);
            printf("Exec failed\n");
            exit(1);
        } else {
            wait(&status);
            if(WIFEXITED(status)){
                int a = WEXITSTATUS(status);
                if(a == 0){
                    printf("Executed properly\n");
                } else {
                    printf("Execution failed\n");
                }
            }
        }
    }
    return 0;
}