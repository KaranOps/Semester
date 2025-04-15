#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdbool.h>
int main(){
        printf("Start\n");
        int i=1;

        while(true){
                char ch[500];
                pid_t p;
                scanf("%[^\n]%*c", ch);
                p = fork();
                if(p==0){
                        printf("Child Process Starts\n");
                        printf("%s\n",ch);
                        exit(0);
                }
        }

}