#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 1024
#define DATA_SIZE 1020

void copy_chain(int fdfrom, int start_block, int fdto) {
    int current_block = start_block;
    char buffer[BLOCK_SIZE];

    while (current_block != -1) {
        off_t offset = (off_t) current_block * BLOCK_SIZE;

        // Move to the correct block
        if (lseek(fdfrom, offset, SEEK_SET) == -1) {
            perror("lseek");
            exit(1);
        }

        // Read one block (1024 bytes)
        if (read(fdfrom, buffer, BLOCK_SIZE) != BLOCK_SIZE) {
            perror("read");
            exit(1);
        }

        // Write the first 1020 bytes to the output file
        if (write(fdto, buffer, DATA_SIZE) != DATA_SIZE) {
            perror("write");
            exit(1);
        }

        // Extract next block number from the last 4 bytes
        memcpy(&current_block, buffer + DATA_SIZE, sizeof(int));
    }
}