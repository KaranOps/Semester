#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<string.h>
#include<errno.h>


int main(int argc,char *argv[]){

	char *file_name = argv[1];
	int project_id = atoi(argv[2]);
	char *operation = argv[3];
	int semnum = atoi(argv[4]);

	key_t key = ftok(file_name,project_id);
	if(key==-1){
		perror("ftok() failed");
		exit(EXIT_FAILURE);
	}

	int semid = semget(key,semnum,IPC_CREAT | 0666);
	if(semid == -1){
		perror("semget() failed");
		exit(EXIT_FAILURE);
	}

	printf("Semaphore bnn gya\n");
	printf("Details of semaphore:\n");
	printf("File: %s\n", file_name);
	printf("Semaphore set ID: %d\n", semid);
	printf("Number of Semaphores: %d\n", semnum);

	return 0;
}
