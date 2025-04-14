#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to hold the metadata of the file system
typedef struct {
    int n;    // Total number of blocks
    int s;    // Size of each block in bytes
    int ubn;  // Number of used blocks
    int fbn;  // Number of free blocks
    unsigned char ub[256]; // Bitmap (can support up to 2048 blocks)
    // Padding to make the structure exactly 4096 bytes
    char padding[4096 - 4*sizeof(int) - 256];
} FileMetadata;

/**
 * Creates and initializes a file as a collection of blocks
 * 
 * @param fname Name of the file to create
 * @param bsize Size of each block in bytes
 * @param bno Number of blocks in the file
 * @return 0 on success, -1 on failure
 */
int init_File_dd(const char *fname, int bsize, int bno) {
    FILE *file = fopen(fname, "wb");
    if (!file) {
        perror("Failed to create file");
        return -1;
    }
    
    // Initialize metadata
    FileMetadata metadata;
    metadata.n = bno;
    metadata.s = bsize;
    metadata.ubn = 0;
    metadata.fbn = bno;
    
    // Initialize bitmap with all zeros (all blocks are free)
    memset(metadata.ub, 0, sizeof(metadata.ub));
    
    // Initialize padding with zeros
    memset(metadata.padding, 0, sizeof(metadata.padding));
    
    // Write metadata to file
    if (fwrite(&metadata, sizeof(metadata), 1, file) != 1) {
        perror("Failed to write metadata");
        fclose(file);
        return -1;
    }
    
    // Create empty blocks filled with zeros
    char *emptyBlock = (char *)calloc(bsize, 1);
    if (!emptyBlock) {
        perror("Memory allocation failed");
        fclose(file);
        return -1;
    }
    
    // Write each block to the file
    for (int i = 0; i < bno; i++) {
        if (fwrite(emptyBlock, bsize, 1, file) != 1) {
            perror("Failed to write block");
            free(emptyBlock);
            fclose(file);
            return -1;
        }
    }
    
    free(emptyBlock);
    fclose(file);
    printf("File '%s' initialized with %d blocks of size %d bytes each\n", fname, bno, bsize);
    return 0;
}

/**
 * Finds a free block, marks it as used, and returns its block number
 * 
 * @param fname Name of the file
 * @return Block number of the allocated block, or -1 on failure
 */
int get_freeblock(const char *fname) {
    FILE *file = fopen(fname, "rb+");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }
    
    // Read metadata
    FileMetadata metadata;
    if (fread(&metadata, sizeof(metadata), 1, file) != 1) {
        perror("Failed to read metadata");
        fclose(file);
        return -1;
    }
    
    // Check if there are any free blocks
    if (metadata.fbn <= 0) {
        printf("No free blocks available\n");
        fclose(file);
        return -1;
    }
    
    // Find the first free block
    int freeBlockNum = -1;
    for (int i = 0; i < metadata.n; i++) {
        int byteIndex = i / 8;
        int bitPosition = i % 8;
        
        // Check if the bit is 0 (block is free)
        if ((metadata.ub[byteIndex] & (1 << bitPosition)) == 0) {
            freeBlockNum = i;
            break;
        }
    }
    
    if (freeBlockNum == -1) {
        printf("Inconsistency: fbn > 0 but no free blocks found\n");
        fclose(file);
        return -1;
    }
    
    // Mark the block as used in the bitmap
    int byteIndex = freeBlockNum / 8;
    int bitPosition = freeBlockNum % 8;
    metadata.ub[byteIndex] |= (1 << bitPosition);
    
    // Update counters
    metadata.ubn++;
    metadata.fbn--;
    
    // Write updated metadata back to file
    fseek(file, 0, SEEK_SET);
    if (fwrite(&metadata, sizeof(metadata), 1, file) != 1) {
        perror("Failed to write updated metadata");
        fclose(file);
        return -1;
    }
    
    // Fill the block with 1's
    long blockOffset = sizeof(metadata) + (long)freeBlockNum * metadata.s;
    fseek(file, blockOffset, SEEK_SET);
    
    char *usedBlock = (char *)malloc(metadata.s);
    if (!usedBlock) {
        perror("Memory allocation failed");
        fclose(file);
        return -1;
    }
    
    // Fill block with 1's (0xFF)
    memset(usedBlock, 0xFF, metadata.s);
    
    if (fwrite(usedBlock, metadata.s, 1, file) != 1) {
        perror("Failed to write to block");
        free(usedBlock);
        fclose(file);
        return -1;
    }
    
    free(usedBlock);
    fclose(file);
    printf("Allocated block %d\n", freeBlockNum);
    return freeBlockNum;
}

/**
 * Frees a used block
 * 
 * @param fname Name of the file
 * @param bno Block number to free
 * @return 1 on success, 0 on failure
 */
int free_block(const char *fname, int bno) {
    FILE *file = fopen(fname, "rb+");
    if (!file) {
        perror("Failed to open file");
        return 0;
    }
    
    // Read metadata
    FileMetadata metadata;
    if (fread(&metadata, sizeof(metadata), 1, file) != 1) {
        perror("Failed to read metadata");
        fclose(file);
        return 0;
    }
    
    // Check if block number is valid
    if (bno < 0 || bno >= metadata.n) {
        printf("Invalid block number: %d\n", bno);
        fclose(file);
        return 0;
    }
    
    // Check if the block is already free
    int byteIndex = bno / 8;
    int bitPosition = bno % 8;
    
    if ((metadata.ub[byteIndex] & (1 << bitPosition)) == 0) {
        printf("Block %d is already free\n", bno);
        fclose(file);
        return 0;
    }
    
    // Mark the block as free in the bitmap
    metadata.ub[byteIndex] &= ~(1 << bitPosition);
    
    // Update counters
    metadata.ubn--;
    metadata.fbn++;
    
    // Write updated metadata back to file
    fseek(file, 0, SEEK_SET);
    if (fwrite(&metadata, sizeof(metadata), 1, file) != 1) {
        perror("Failed to write updated metadata");
        fclose(file);
        return 0;
    }
    
    // Fill the block with 0's
    long blockOffset = sizeof(metadata) + (long)bno * metadata.s;
    fseek(file, blockOffset, SEEK_SET);
    
    char *freeBlock = (char *)calloc(metadata.s, 1);
    if (!freeBlock) {
        perror("Memory allocation failed");
        fclose(file);
        return 0;
    }
    
    if (fwrite(freeBlock, metadata.s, 1, file) != 1) {
        perror("Failed to write to block");
        free(freeBlock);
        fclose(file);
        return 0;
    }
    
    free(freeBlock);
    fclose(file);
    printf("Freed block %d\n", bno);
    return 1;
}

/**
 * Checks the integrity of the file system
 * 
 * @param fname Name of the file
 * @return 0 if consistent, 1 if inconsistent
 */
int check_fs(const char *fname) {
    FILE *file = fopen(fname, "rb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }
    
    // Read metadata
    FileMetadata metadata;
    if (fread(&metadata, sizeof(metadata), 1, file) != 1) {
        perror("Failed to read metadata");
        fclose(file);
        return 1;
    }
    
    // Check 1: ubn + fbn should equal n
    if (metadata.ubn + metadata.fbn != metadata.n) {
        printf("Inconsistency: ubn (%d) + fbn (%d) != n (%d)\n", 
               metadata.ubn, metadata.fbn, metadata.n);
        fclose(file);
        return 1;
    }
    
    // Check 2: Count the number of 1's in the bitmap
    int usedBlockCount = 0;
    for (int i = 0; i < metadata.n; i++) {
        int byteIndex = i / 8;
        int bitPosition = i % 8;
        
        if (metadata.ub[byteIndex] & (1 << bitPosition)) {
            usedBlockCount++;
        }
    }
    
    if (usedBlockCount != metadata.ubn) {
        printf("Inconsistency: Bitmap has %d used blocks, but ubn = %d\n", 
               usedBlockCount, metadata.ubn);
        fclose(file);
        return 1;
    }
    
    // Check 3: Verify that used blocks contain 1's and free blocks contain 0's
    char *buffer = (char *)malloc(metadata.s);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }
    
    for (int i = 0; i < metadata.n; i++) {
        int byteIndex = i / 8;
        int bitPosition = i % 8;
        bool isUsed = (metadata.ub[byteIndex] & (1 << bitPosition)) != 0;
        
        // Read the block
        long blockOffset = sizeof(metadata) + (long)i * metadata.s;
        fseek(file, blockOffset, SEEK_SET);
        
        if (fread(buffer, metadata.s, 1, file) != 1) {
            printf("Failed to read block %d\n", i);
            free(buffer);
            fclose(file);
            return 1;
        }
        
        // Check block content
        if (isUsed) {
            // Used blocks should contain all 1's (0xFF)
            for (int j = 0; j < metadata.s; j++) {
                if (buffer[j] != (char)0xFF) {
                    printf("Inconsistency: Block %d is marked as used but doesn't contain all 1's\n", i);
                    free(buffer);
                    fclose(file);
                    return 1;
                }
            }
        } else {
            // Free blocks should contain all 0's
            for (int j = 0; j < metadata.s; j++) {
                if (buffer[j] != 0) {
                    printf("Inconsistency: Block %d is marked as free but doesn't contain all 0's\n", i);
                    free(buffer);
                    fclose(file);
                    return 1;
                }
            }
        }
    }
    
    free(buffer);
    fclose(file);
    printf("File system is consistent\n");
    return 0;
}

/**
 * Main function to demonstrate the file system operations
 */
int main() {
    const char *filename = "dd1";
    int blockSize = 4096;
    int blockCount = 10;  // Using a smaller number for demonstration
    
    printf("=== File System Demonstration ===\n\n");
    
    // Initialize the file
    printf("1. Initializing file '%s'...\n", filename);
    if (init_File_dd(filename, blockSize, blockCount) != 0) {
        printf("Failed to initialize file\n");
        return 1;
    }
    printf("\n");
    
    // Check file system integrity
    printf("2. Checking file system integrity...\n");
    if (check_fs(filename) != 0) {
        printf("File system integrity check failed\n");
        return 1;
    }
    printf("\n");
    
    // Allocate some blocks
    printf("3. Allocating blocks...\n");
    int block1 = get_freeblock(filename);
    int block2 = get_freeblock(filename);
    int block3 = get_freeblock(filename);
    
    if (block1 == -1 || block2 == -1 || block3 == -1) {
        printf("Failed to allocate blocks\n");
        return 1;
    }
    printf("\n");
    
    // Check file system integrity again
    printf("4. Checking file system integrity after allocation...\n");
    if (check_fs(filename) != 0) {
        printf("File system integrity check failed\n");
        return 1;
    }
    printf("\n");
    
    // Free a block
    printf("5. Freeing block %d...\n", block2);
    if (free_block(filename, block2) != 1) {
        printf("Failed to free block %d\n", block2);
        return 1;
    }
    printf("\n");
    
    // Check file system integrity again
    printf("6. Checking file system integrity after freeing a block...\n");
    if (check_fs(filename) != 0) {
        printf("File system integrity check failed\n");
        return 1;
    }
    printf("\n");
    
    // Try to allocate another block (should reuse the freed block)
    printf("7. Allocating another block (should reuse block %d)...\n", block2);
    int block4 = get_freeblock(filename);
    if (block4 == -1) {
        printf("Failed to allocate another block\n");
        return 1;
    }
    printf("\n");
    
    // Final integrity check
    printf("8. Final integrity check...\n");
    if (check_fs(filename) != 0) {
        printf("File system integrity check failed\n");
        return 1;
    }
    
    printf("\n=== Demonstration completed successfully ===\n");
    return 0;
}
