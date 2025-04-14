#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include <sys/wait.h>

void reverse(char *s){
        int len = strlen(s);
        for(int i=0;i<len/2;i++){
                char temp = s[i];
                s[i] = s[len-i-1];
                s[len-i-1] = temp;
        }
}
int main(int argc,char *argv[]){
        pid_t p;
        //printf("Start");

        int i=0;
        for(i=1;i<argc;i++){
                p = fork();
                if(p==0){
                        char *s = argv[i];
                        reverse(s);
                        printf("String s%d %s\n", i,s);                        exit(0);
                }
        }

}