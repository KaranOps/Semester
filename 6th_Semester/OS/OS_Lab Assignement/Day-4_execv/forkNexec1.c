#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdbool.h>

int main(){
        printf("Start\n");
        int i=1;

        while(true){
                char ch[500];
                scanf("%[^\n]%*c", ch);
                printf("%s\n", ch);
        }

}