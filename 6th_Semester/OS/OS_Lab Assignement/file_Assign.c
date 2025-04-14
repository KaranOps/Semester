#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define MAXBLOCK 2048

typedef struct block
{
        char val[4096];
} Block;

typedef struct zerothBlock
{
        int noofBlocks;
        int sizeofBlocks;
        int ubn;
        int fbn;
        char ub[256];
} info;

int initFile(char *filename, int blsize, int blno)
{
        int ofd = open("file1.txt", O_CREAT | O_RDWR, 0700);
        if (ofd == -1)
        {
                perror("File creation Failed");
                return -1;
        }
        info blockzero;
        blockzero.noofBlocks = blno;
        blockzero.sizeofBlocks = blsize;
        blockzero.ubn = 0;
        blockzero.fbn = 2048;
        memset(blockzero.ub, '0', sizeof(blockzero.ub)); // initialize all blocks with 0;

        if (write(ofd, &blockzero, sizeof(info)) != sizeof(info))
        {
                perror("Metadata write failed");
                close(ofd);
                return -1;
        }

        Block buffer;
        memset(buffer.val, '0', sizeof(buffer.val));
        for (int i = 0; i < blockzero.noofBlocks; i++)
        {
                if (write(ofd, &buffer, sizeof(Block)) != sizeof(Block))
                {
                        perror("Block write failed");
                        close(ofd);
                        return -1;
                }
        }

        close(ofd);
        return 0;
};

int get_freeblock(char *filename)
{
        int fd = open("file1.txt", O_CREAT | O_RDWR, 0700);
        if (fd == -1)
        {
                perror("File open Failed");
                return -1;
        }

        info blockzero;
        if (read(fd, &blockzero, sizeof(info)) != sizeof(info))
        {
                perror("Metadata read failed");
                close(fd);
                return -1;
        }
        if (blockzero.fbn == 0)
        {

                fprintf(stderr, "No free blocks available\n");
                close(fd);
                return -1;
        }
        int firstFreeBlock = -1;
        for (int i = 0; i < blockzero.noofBlocks; i++)
        {
                int byteInd = i / 8;
                int bitInd = i % 8;
                if ((blockzero.ub[byteInd] & (1 << bitInd)) == 0)
                {
                        firstFreeBlock = i;
                        blockzero.ub[byteInd] |= (1 << bitInd);
                        break;
                }
        }
        if (firstFreeBlock == -1)
        {
                fprintf(stderr, "Error: No free block found\n");
                close(fd);
                return -1;
        }

        blockzero.ubn++;
        blockzero.fbn--;
        printf("used block is %d freeblock is %d\n", blockzero.ubn, blockzero.fbn);
        if (lseek(fd, 0, SEEK_SET) == -1)
        {
                perror("lseek failed");
                close(fd);
                return -1;
        }
        if (write(fd, &blockzero, sizeof(info)) != sizeof(info))
        {
                perror("Metadata write failed");
                close(fd);
                return -1;
        }

        Block buffer;
        memset(buffer.val, '1', sizeof(buffer.val));

        off_t blockPosition = 4096 + firstFreeBlock * sizeof(Block);
        lseek(fd, blockPosition, SEEK_SET);

        if (write(fd, &buffer, sizeof(Block)) != sizeof(Block))
        {
                perror("Block write failed");
                close(fd);
                return -1;
        }

        close(fd);
        return firstFreeBlock;
}

int free_block(const char *fname, int blockNo)
{

        int fd = open(fname, O_RDWR);
        if (fd == -1)
        {
                perror("File open failed");
                return 0;
        }
        info blockzero;
        if (read(fd, &blockzero, sizeof(info)) != sizeof(info))
        {
                perror("Metadata read failed");
                close(fd);
                return 0;
        }
        if (blockNo < 0 || blockNo >= blockzero.noofBlocks)
        {
                fprintf(stderr, "Invalid block number\n");
                close(fd);
                return 0;
        }
        int byteInd = blockNo / 8;
        int bitInd = blockNo % 8;
        if (blockzero.ub[byteInd] & (1 << bitInd) == 0) // blockNoth block is already free
        {
                fprintf(stderr, "Block %d is already free\n", blockNo);
                close(fd);
                return 0;
        }

        blockzero.ub[byteInd] &= ~(1 << bitInd); // blockNo th block is freed
        blockzero.ubn--;
        blockzero.fbn++;
        printf("used block is %d freeblock is %d\n", blockzero.ubn, blockzero.fbn);
        if (lseek(fd, 0, SEEK_SET) == -1 || write(fd, &blockzero, sizeof(info)) != sizeof(info))
        {
                perror("Metadata write failed");
                close(fd);
                return 0;
        }
        Block buffer;
        memset(buffer.val, '0', sizeof(buffer.val));
        off_t blockPosition = 4096 + blockNo * sizeof(Block);
        lseek(fd, blockPosition, SEEK_SET);
        if (write(fd, &buffer, sizeof(Block)) != sizeof(Block))
        {
                perror("Block write failed");
                close(fd);
                return -1;
        }

        close(fd);
        return 1;
}

int count_set_bits(const char *bitmap, int size)
{
        int count = 0;
        for (int i = 0; i < size; i++)
        {
                unsigned char byte = bitmap[i];
                while (byte)
                {
                        count += (byte & 1);
                        byte >>= 1;
                }
        }
        return count;
}

int check_fs(const char *fname)
{
        int fd = open(fname, O_RDONLY);
        if (fd == -1)
        {
                perror("File open failed");
                return 1;
        }
        info blockzero;
        if (read(fd, &blockzero, sizeof(info)) != sizeof(info))
        {
                perror("Metadata read failed");
                close(fd);
                return 1;
        }
        printf("ubn is %d and fbn is %d\n", blockzero.ubn, blockzero.fbn);
        if (blockzero.ubn + blockzero.fbn != blockzero.noofBlocks)
        {
                perror("Error: Block count mismatch (ubn + fbn != n)");
                close(fd);
                return 1;
        }
        int actualUsedBlocks = count_set_bits(blockzero.ub, 256);
        if (actualUsedBlocks != blockzero.ubn)
        {
                perror("Error: Bitmap inconsistency (actualUsedBlocks != ubn)");
                close(fd);
        }
        close(fd);
        return 0;
}

int main()
{
        int fd;
        fd = initFile("file1.txt", 4096, 2048);
        if (fd == 0)
                printf("initialization done\n");
        else
                printf("initialization failed\n");

        for (int i = 0; i < 3; i++)
        {
                int freeB = get_freeblock("file1.txt");
                if (freeB == -1)
                        printf("Failed to allocate a block (possibly no free blocks left).\n");
                else
                        printf("Allocated block: %d\n", freeB);
        }

        for (int i = 0; i < 2; i++)
        {
                int blockNo;
                printf("Enter blockNo you want to free:");
                scanf("%d", &blockNo);
                int freedBlock = free_block("file1.txt", blockNo);
                if (freedBlock == -1)
                        printf("Failed to free %dth block\n", blockNo);
                else
                        printf("%dth block is freed\n", blockNo);
        }

        int result = check_fs("file1.txt");
        if (result == 0)
        {
                printf("File system is consistent.");
        }
        else
        {
                printf("File system integrity check failed!");
        }
}
