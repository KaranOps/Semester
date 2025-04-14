#include<stdio.h>
#include<unistd.h>

int main(){
        pid_t p;
        printf("Hello World 1\n");
        p = fork();

        if (p == 0) {
                 printf("Child process!");
        } else {
                 printf("Parent process!");
        }
        printf("Hello World 2\n");
}