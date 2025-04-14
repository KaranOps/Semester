#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

typedef struct task{
	char data[100];
	pid_t worker_pid;
	int status;
};

int main(){
	int shmid = shmget(IPC_PRIVATE,sizeof(struct task),IPC_CREAT|0777);
	if(shmid < 0){
		perror("Failed to create shared memory key.\n");
		exit(0);	
	}

	struct task *solve;
	
	for(int i=1;i>0;i++){
		int i;
		int len = rand()%100;
		for(i=0; i<len;i++){
			solve->data[i] = rand()%10;
		}
		solve->data[i] = '\0';
		solve->data = 1;
		while(solve->status != 3){
			if(solve->status == 3){
				solve->status = 4;
				printf("Length of the array: ",sove->data);
				printf("Process id of the worker %d.\n",solve->worder_pid);
			}
		}
		

	}
}
