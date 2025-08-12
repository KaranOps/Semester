#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define NUM_POINTS 1000000

int main(){
        srand(time(NULL));//NULL becoz in start it takes the current time value

        //Setting the upper bound and lower bound
        int min_value = 1;
        int max_value = 10;

        //Excel read csv file that's why we are taking csv file
        FILE *file = fopen("uniform_data.csv", "w"); //Open file for writing

        if(file==NULL){ //If file is sucessfully opened and is empty
                fprintf(stderr,"Error opening file\n");
                return 1;
        }

        for(int i=0;i<NUM_POINTS;++i){
                //Generate number between 1 to 10
                int random_number = min_value + rand() % (max_value-min_value+1);
                //writing 100 random numbers in file
                fprintf(file, "%d\n", random_number);
        }

        //Close the file
        fclose(file);

        printf("Data has been written in uniform_data.txt\n");

        return 0;
}

