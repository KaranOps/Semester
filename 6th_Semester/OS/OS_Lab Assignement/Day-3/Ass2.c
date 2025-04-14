#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include <sys/wait.h>

void bubbleSort(int arr[], int n) {
    int i, j, temp;
    for (i = 0; i < n - 1; i++) {
         for (j = 0; j < n - i - 1; j++) {
             if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
int solve(int arr[],int i,int n){
        bubbleSort(arr,n);
        return arr[i];
}
int main(){
        pid_t p;
        int n;
        scanf("%d\n",&n);

        int arr[n];
        for(int i=0;i<n;i++){
                scanf("%d\n", &arr[i]);
        }

        for(int i=0;i<n;i++){
                p = fork();
                                   if(p==0){
                        int maxi = solve(arr,i,n);
                        printf("%d %d\t",i, maxi);
                        exit(0);
                }
        }
}