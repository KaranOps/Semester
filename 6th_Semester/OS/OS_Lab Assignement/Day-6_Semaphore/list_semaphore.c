void list_processes(int semid, int semnum) {

    int waiting_for_increase, waiting_for_zero;
    pid_t last_process;
    
    // Get number of processes waiting for the semaphore value to increase
    waiting_for_increase = semctl(semid, semnum, GETNCNT);
    if (waiting_for_increase == -1) {
        perror("semctl GETNCNT failed");
        exit(EXIT_FAILURE);
    }
    
    // Get number of processes waiting for the semaphore value to become zero
    waiting_for_zero = semctl(semid, semnum, GETZCNT);
    if (waiting_for_zero == -1) {
        perror("semctl GETZCNT failed");
        exit(EXIT_FAILURE);
    }
    
    // Get PID of last process that operated on this semaphore
    last_process = semctl(semid, semnum, GETPID);
    if (last_process == -1) {
        perror("semctl GETPID failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Processes waiting on semaphore %d:\n", semnum);
    printf("Waiting for increase: %d\n", waiting_for_increase);
    printf("Waiting for zero: %d\n", waiting_for_zero);
    printf("Last process to operate: %d\n", last_process);
}