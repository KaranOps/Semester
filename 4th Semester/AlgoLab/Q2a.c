#include<stdio.h>
#include<stdlib.h>

#define MAX_LINE_LENGTH 1024
#define MAX_NUM_VALUES 1000


void merge(double arr[],int l,int mid,int r){
    int temp[r-l+1]; //Taking temproary array to store the sorted array and then copy to main array
    int left=l;
    int right=mid+1;
    int i=0;


    while(left<= mid && right<=r){
        if(arr[left] < arr[right]){
            temp[i++]=arr[left];
            left++;
        }
        else{
            temp[i++]=arr[right];
            right++;
        }
    }

    //If any value is left in left part of arr
    while(left<=mid){
        temp[i++]=arr[left];
        left++;
    }

    //If any value is right in left part of arr
    while(right <= r){
        temp[i++]=arr[right];
        right++;
    }

    //Now copying all values to main array 'arr'
    for(int j=l;j<=r;j++){
        arr[j] = temp[j-l];
    }
    
}

void mergeSort(double arr[], int l, int r){
    if(l>=r){
        return;
    }

    int mid = l+(r-l)/2;

    //Sorting first and second half
    mergeSort(arr,l,mid);
    mergeSort(arr,mid+1,r);

    merge(arr,l,mid,r);
}

// Function to read data from a CSV file
int readCSV(const char *filename, double arr[], int maxNumValues) {

    //Openeing the file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return -1; // Return -1 on error
    }


    int count = 0;
    char line[MAX_LINE_LENGTH];

    //fgets used for taking input of string in C
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL && count < maxNumValues) {//agar file me kuch na ho then fgets !=NULL
        arr[count] = atof(line); //'atof()' function converts string to double
        count++;
    }

    fclose(file);
    return count; // Return the number of values read
}

// Function to write data to a CSV file
void writeCSV(const char *filename, double arr[], int numValues) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing: %s\n", filename);
        return;
    }

    for (int i = 0; i < numValues; i++) {
        fprintf(file, "%.6f\n", arr[i]);
    }

    fclose(file);
}


int main(){
    const char *inputFileName = "uniform_data.csv";
    const char *outputFileName = "mergeUd_data.csv";
    double data[MAX_NUM_VALUES];

    // Read data from CSV file
    int numValues = readCSV(inputFileName, data, MAX_NUM_VALUES);
    if (numValues < 0) {
        return 1; // Error reading file
    }

    // Perform merge sort on the data
    mergeSort(data, 0, numValues);

    // Write sorted data to CSV file
    writeCSV(outputFileName, data, numValues);

    printf("Data has been sorted and written to %s\n", outputFileName);

    return 0;
}