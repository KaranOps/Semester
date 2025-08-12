#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define NUM_MAX 100

int main(){
        srand(time(NULL));

        int min_value = 1;
        int max_value = 10;

        FILE *file = fopen("normalDist.csv","w");

        fprintf(file, "values\n");

        for(int i=0;i<NUM_MAX;i++){
                int sum = 0;
                for(int j=0;j<min_value;j++){
                        sum += rand() % (max_value/min_value);
                }

                fprintf(file, "%d\n", sum);
        }
                printf("Data is written in normalDist.csv\n");
        return 0;
}

