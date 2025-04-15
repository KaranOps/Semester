#include <stdio.h>
#include <sys/types.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/ipc.h>   /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/sem.h>   /* for semget(2) semop(2) semctl(2) */
#include <unistd.h>    /* for fork(2) */
#include <string.h>
#include <stdlib.h> /* for exit(3) */

union semun
{
        int val;               // for SETVAL
        struct semid_ds *buf;  // for IPC SET and IPC_STAT
        unsigned short *array; // for GETALL SETALL
};

int createSemaphore(char *fileName, int projectID, int semnum)
{
        key_t key = ftok(fileName, projectID);
        if (key == -1)
        {
                perror("ftok() failed");
                exit(1);
        }
        printf("key value is %d\n", key);
        // create semaphore set
        int semID = semget(key, semnum, IPC_CREAT | 0777);
        printf("semId value is %d semnum = %d\n", semID, semnum);
        if (semID == -1)
        {
                perror("semget() failed");
                exit(1);
        }
        // print detail of semaphore
        printf("semaphore created succesfully\n");
        printf("File: %s\nProjectID: %d\nSempahoresCount: %d\nkey: %d\nSemaphoreID:%d\n", fileName, projectID, semnum, key, semID);
        return semID;
}
void setSemaphoreValue(int semID, int setInd, int setVal)
{
        union semun sem_union;
        sem_union.val = setVal;
        if (semctl(semID, setInd, SETVAL, sem_union.val) == -1)
        {
                perror("semctl SETVAL failed");
                exit(1);
        }
        printf("%dth index semaphore value set to %d\n", setInd, setVal);
}

int getSemVal(int semID, int getInd)
{
        union semun sem_union;
        int semVal = semctl(semID, getInd, GETVAL);
        if (semVal == -1)
        {
                perror("semctl GETVAL failed");
                exit(1);
        }
        printf("%dth index semaphore value is: %d\n", getInd, semVal);
        return semVal;
}

void incSemVal(int semID, int ind, int incval)
{
        union semun sem_union;
        int currVal = getSemVal(semID, ind);
        int newVal = currVal + incval;
        setSemaphoreValue(semID, ind, newVal);
}

void decSemVal(int semID, int ind, int decval)
{
        union semun sem_union;
        int currVal = getSemVal(semID, ind);
        int newVal = currVal - decval;
        setSemaphoreValue(semID, ind, newVal);
}
void destroySem(int semID, int semnum)
{
        if (semctl(semID, 0, IPC_RMID) == -1)
        {
                perror("semctl IPC_RMID failed");
                exit(1);
        }
        printf("Semaphore set removed.\n");
}

int main(int argc, char *argv[])
{

        if (argc != 5 || strcmp(argv[3], "create") != 0)
        {
                fprintf(stderr, "Usage: %s <file_name> <project_id> create <semnum>\n", argv[0]);
                exit(1);
        }
        char *fileName = argv[1];
        int projectID = atoi(argv[2]);
        int semnum = atoi(argv[4]);
        int semID = createSemaphore(fileName, projectID, semnum);

        // SETVAL
        int setInd, setVal;
        printf("Enter index no. of semaphore and set value of that :");
        scanf("%d %d", &setInd, &setVal);
        if (setInd >= semnum)
        {
                perror("semInd doesn't exist\n");
                exit(1);
        }
        else
        {
                setSemaphoreValue(semID, setInd, setVal);
        }

        // GETVAL
        int getInd;
        printf("Enter index no. of semaphore to get that value :");
        scanf("%d", &getInd);
        if (getInd >= semnum)
        {
                perror("semInd doesn't exist\n");

                exit(1);
        }
        else
        {
                getSemVal(semID, getInd);
        }

        // INCREASE SEMAPHORE VALUE;
        int semindex, incval;
        printf("Enter index no. of semaphore and increment value of that:");
        scanf("%d %d", &semindex, &incval);
        if (semindex >= semnum)
        {
                perror("semInd doesn't exist\n");
                exit(1);
        }
        else
        {
                incSemVal(semID, semindex, incval);
        }

        // DECREASE SEMAPHORE VALUE;
        int Ind, decval;
        printf("Enter index no. of semaphore and value by which you want to decrease:");
        scanf("%d %d", &Ind, &decval);
        if (Ind >= semnum)
        {
                perror("semInd doesn't exist\n");
                exit(1);
        }
        else
        {
                decSemVal(semID, Ind, decval);
        }

        // Destroy semaphore set
        destroySem(semID, semnum);
        return 0;
}
