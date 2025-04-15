#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

int shmid; // Shared memory ID

// Signal handler to release shared memory
void releaseSHM(int signum) {
    int status = shmctl(shmid, IPC_RMID, NULL);
    if (status == 0) {
        fprintf(stderr, "Shared memory with id = %d removed successfully.\n", shmid);
    } else {
        perror("Failed to remove shared memory");
    }
    exit(0);
}

// Function to calculate factorial
int factorial(int n) {
    if (n == 0 || n == 1) return 1;
    int fact = 1;
    for (int i = 2; i <= n; i++) {
        fact *= i;
    }
    return fact;
}

int main() {
    int *shared_data; // Pointer to shared memory
    pid_t pid;
    sighandler_t shandler;

    // Install signal handler for cleanup
    shandler = signal(SIGINT, releaseSHM);

    // Create shared memory
    shmid = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget() failed");
        exit(1);
    }

    // Attach shared memory
    shared_data = (int *)shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat() failed");
        exit(1);
    }

    // Initialize shared memory
    shared_data[0] = 0; // Shared variable `n`
    shared_data[1] = 0; // Flag: 0 = parent writes, 1 = child writes

    // Fork a child process
    pid = fork();
    if (pid == 0) { // Child process
        while (1) {
            if (shared_data[1] == 1) { // Check if parent has written a new number
                int n = shared_data[0];
                printf("Child: Received number %d from parent.\n", n);
                int fact = factorial(n);
                shared_data[0] = fact; // Write factorial back to shared memory
                printf("Child: Calculated factorial %d and wrote it back.\n", fact);
                shared_data[1] = 0; // Signal parent to write next number
            }
        }
    } else { // Parent process
        srand(time(NULL)); // Seed for random number generation
        while (1) {
            if (shared_data[1] == 0) { // Check if child has written the factorial
                int random_num = rand() % 10 + 1; // Generate random number (1-10)
                shared_data[0] = random_num; // Write random number to shared memory
                printf("Parent: Wrote number %d to shared memory.\n", random_num);
                shared_data[1] = 1; // Signal child to calculate factorial
                sleep(1); // Simulate delay
                if (shared_data[1] == 0) { // Check if child has written the factorial
                    printf("Parent: Read factorial %d from shared memory.\n", shared_data[0]);
                }
            }
        }
        wait(NULL); // Wait for child process to finish
    }

    // Detach and release shared memory
    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}