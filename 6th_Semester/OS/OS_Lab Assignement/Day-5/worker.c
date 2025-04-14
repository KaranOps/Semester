#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.get>
#include <sys/ipc.h>

int main(){

	key_t shmkey = ftok(/home/hanau1/student/btech2022/karank/OS_Lab/Ass_4Feb/server.c,1);
	int shmid = shmget(shmkey,size_t 150,IPC_CREAT|0777);
	shmat(shmid);
	while(solve->status != -1){
		while(solve->status == 1){
			solve->status = 2;
			solve->worker_pid = getpid();
			int len = 0;
			for(int i=0; solve->data[i] != '\0';i++){
				len++;
			}

			solve->data[0] = len + '0';
			solve->status = 3;
		}
	}
}
