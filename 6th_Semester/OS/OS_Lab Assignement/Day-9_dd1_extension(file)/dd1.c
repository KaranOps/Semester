#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BlockSize 4096

typedef struct block
{
    char val[4096];
} Block;

typedef struct metadata
{
    char name[12];
    int size;
} info;

int mymkfs(char *filename)
{
    FILE *fs = fopen(filename, "wb");
    if (fs == NULL)
    {
        perror("creation of file failed");
        return -1;
    }
    int totalSize = (8 + 2048) * 4096;

    // ek block ke liye buffer allocate karege
    char *block = (char *)calloc(BlockSize, 1);
    if (block == NULL)
    {
        perror("Allocation failed");
        return -1;
    }

    // 2048+8 empty blocks file me dalegee
    for (int i = 0; i < (2048 + 8); i++)
    {
        size_t status = fwrite(block, BlockSize, 1, filename);
        if (status != 1)
        {
            perror("Writing failed");
            free(block);
            fclose(fs);
            return -1;
        }
    } // free karege block ko
    free(block);
    // close karege file ko
    fclose(fs);

    printf("\nFile creation successfully\n");
    return 0;
}

// function to find the freeslot in file
int freeSlot(FILE *fs)
{
    
}

// function to copy linux file to dd1

// function to find the file(jisko copy krna hai linux file me)

// function to copy from dd1 to linux file

int main()
{


    return 0;
}