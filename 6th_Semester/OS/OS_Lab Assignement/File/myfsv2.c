#include <stdio.h>
#include <string.h>        /* strcmp(), strrchr() */
#include <stdlib.h>        /* exit() */
#include <fcntl.h>         /* open() */
#include <errno.h>         /* perror() */
#include <unistd.h>        /* write() close() */
#include <sys/stat.h>      /* stat() */
#include <sys/sysmacros.h> /* stat() */
#include <stdint.h>        /* stat() */

// Define filesystem structures
typedef struct
{
        int block_size;
        int num_blocks;
        int free_blocks;
        int root_dir_block;
        char fs_name[32];
        // Add other filesystem metadata as needed
} SuperBlock;

typedef struct
{
        char name[256];
        int size;
        int is_directory;
        int first_block;
        int num_blocks;
        time_t created_time;
        time_t modified_time;
} FileEntry;

char buf[4096];
char sbuf[8 * 4096];

// Function
int mymkfs(const char *fname, int block_size, int no_of_blocks);
int mycopyTo(const char *fname, char *myfname);
int mycopyFrom(char *myfname, const char *fname);
int myrm(char *myfname);
int mymkdir(char *mydirname);
int myrmdir(char *mydirname);
int myreadBlock(char *myfname, char *buf, int block_no);
int mystat(char *myname, char *buf);

// Helper functions
int search(char *myfname, char **myfile_name, char **myfs_name);
int find_file(int fd, const char *filename, FileEntry *entry);
int allocate_blocks(int fd, int num_blocks);
void update_superblock(int fd, SuperBlock *sb);

int main(int argc, char *argv[])
{
        if (argc < 2)
        {
                fprintf(stderr, "Usage:\n");
                fprintf(stderr, "  %s mymkfs <fname> <block_size> <no_of_blocks>\n", argv[0]);
                return 1;
        }

        if (strcmp(argv[1], "mymkfs") == 0)
        {
                if (argc != 5)
                {
                        fprintf(stderr, "Usage: %s mymkfs <fname> <block_size> <no_of_blocks>\n", argv[0]);
                        return 1;
                }
                return mymkfs(argv[2], atoi(argv[3]), atoi(argv[4]));
        }
        else if (strcmp(argv[1], "mycopyTo") == 0)
        {
                if (argc != 4)
                {
                        fprintf(stderr, "Usage: %s mycopyTo <fname> <myfile name>@<myfs file name>\n", argv[0]);
                        return 1;
                }
                return mycopyTo(argv[2], argv[3]);
        }
        else if (strcmp(argv[1], "mycopyFrom") == 0)
        {
                if (argc != 4)
                {
                        fprintf(stderr, "Usage: %s mycopyFrom <myfile name>@<myfs file name> <fname>\n", argv[0]);
                        return 1;
                }
                return mycopyFrom(argv[2], argv[3]);
        }
        else
        {
                fprintf(stderr, "Unknown command: %s\n", argv[1]);
                return 1;
        }

        return 0;
}

int mymkfs(const char *fname, int block_size, int no_of_blocks)
{
        FILE *fp;
        SuperBlock sb;

        // Open the file for writing
        fp = fopen(fname, "wb");
        if (fp == NULL)
        {
                perror("Failed to create filesystem file");
                return -1;
        }

        // Initialize superblock
        sb.block_size = block_size;
        sb.num_blocks = no_of_blocks;

        // Write superblock (fixed size of 4096 bytes)
        char superblock_data[4096] = {0};
        memcpy(superblock_data, &sb, sizeof(SuperBlock));
        fwrite(superblock_data, 1, 4096, fp);

        // Initialize all blocks
        char *empty_block = calloc(1, block_size);
        if (empty_block == NULL)
        {
                perror("Memory allocation failed");
                fclose(fp);
                return -1;
        }

        // Write empty blocks
        for (int i = 0; i < no_of_blocks; i++)
        {
                fwrite(empty_block, 1, block_size, fp);
        }

        free(empty_block);
        fclose(fp);

        printf("Created myfsv2 filesystem on %s with %d blocks of size %d bytes\n",
               fname, no_of_blocks, block_size);
        return 0;
}
int search(char *myfname, char **myfile_name, char **myfs_name)
{
        // search myfname to get myfile name and myfs file name
        char *at_sign = strchr(myfname, '@');
        if (at_sign == NULL)
        {
                fprintf(stderr, "Invalid myfname format. Expected <myfile name>@<myfs file name>\n");
                return -1;
        }

        // Split the string at '@'
        *at_sign = '\0';
        *myfile_name = myfname;
        *myfs_name = at_sign + 1;

        return 0;
}

int mycopyTo(const char *fname, char *myfname)
{
        int fd_src, fd_myfs;
        struct stat st;
        char *myfile_name, *myfs_name;
        SuperBlock sb;
        FileEntry file_entry;
        int blocks_needed, first_block;
        int bytes_read, bytes_written;

        // traverse the path
        if (search(myfname, &myfile_name, &myfs_name) != 0)
        {
                return -1;
        }

        // Open source file
        fd_src = open(fname, O_RDONLY);
        if (fd_src == -1)
        {
                fprintf(stderr, "%s: ", fname);
                perror("Source file cannot be opened for reading");
                return -1;
        }

        // Get source file size
        if (fstat(fd_src, &st) == -1)
        {
                fprintf(stderr, "%s: ", fname);
                perror("Failed to get file stats");
                close(fd_src);
                return -1;
        }

        // Open myfs file
        fd_myfs = open(myfs_name, O_RDWR);
        if (fd_myfs == -1)
        {
                fprintf(stderr, "%s: ", myfs_name);
                perror("Myfs file cannot be opened");
                close(fd_src);
                return -1;
        }

        // Read superblock
        if (read(fd_myfs, &sb, sizeof(SuperBlock)) != sizeof(SuperBlock))
        {
                fprintf(stderr, "%s: ", myfs_name);
                perror("Failed to read superblock");
                close(fd_src);
                close(fd_myfs);
                return -1;
        }

        // Calculate number of blocks needed
        blocks_needed = (st.st_size + sb.block_size - 1) / sb.block_size;

        // Check if there are enough free blocks
        if (blocks_needed > sb.free_blocks)
        {
                fprintf(stderr, "Not enough free blocks in filesystem\n");
                close(fd_src);
                close(fd_myfs);
                return -1;
        }

        // Allocate blocks for the file
        first_block = allocate_blocks(fd_myfs, blocks_needed);
        if (first_block == -1)
        {
                fprintf(stderr, "Failed to allocate blocks\n");
                close(fd_src);
                close(fd_myfs);
                return -1;
        }

        // Update superblock free blocks count
        sb.free_blocks -= blocks_needed;
        update_superblock(fd_myfs, &sb);

        // Create file entry
        memset(&file_entry, 0, sizeof(FileEntry));
        strncpy(file_entry.name, myfile_name, 255);
        file_entry.name[255] = '\0';
        file_entry.size = st.st_size;
        file_entry.is_directory = 0;
        file_entry.first_block = first_block;
        file_entry.num_blocks = blocks_needed;
        file_entry.created_time = time(NULL);
        file_entry.modified_time = time(NULL);

        // Write file entry to directory
        // write it at the root directory block
        lseek(fd_myfs, sb.root_dir_block * sb.block_size, SEEK_SET);
        write(fd_myfs, &file_entry, sizeof(FileEntry));

        // Copy file data
        lseek(fd_myfs, first_block * sb.block_size, SEEK_SET);
        while ((bytes_read = read(fd_src, buf, sb.block_size)) > 0)
        {
                bytes_written = write(fd_myfs, buf, bytes_read);
                if (bytes_written != bytes_read)
                {
                        fprintf(stderr, "Failed to write data to myfs\n");
                        close(fd_src);
                        close(fd_myfs);
                        return -1;
                }
        }

        close(fd_src);
        close(fd_myfs);

        printf("Copied %s to %s@%s\n", fname, myfile_name, myfs_name);
        return 0;
}

int mycopyFrom(char *myfname, const char *fname)
{
        int fd_dest, fd_myfs;
        char *myfile_name, *myfs_name;
        SuperBlock sb;
        FileEntry file_entry;
        int bytes_read, bytes_written, remaining;

        // Parse the path
        if (search(myfname, &myfile_name, &myfs_name) != 0)
        {
                return -1;
        }

        // Open myfs file
        fd_myfs = open(myfs_name, O_RDONLY);
        if (fd_myfs == -1)
        {
                fprintf(stderr, "%s: ", myfs_name);
                perror("Myfs file cannot be opened");
                return -1;
        }

        // Read superblock
        if (read(fd_myfs, &sb, sizeof(SuperBlock)) != sizeof(SuperBlock))
        {
                fprintf(stderr, "%s: ", myfs_name);
                perror("Failed to read superblock");
                close(fd_myfs);
                return -1;
        }

        // Find file entry
        if (find_file(fd_myfs, myfile_name, &file_entry) != 0)
        {
                fprintf(stderr, "File %s not found in %s\n", myfile_name, myfs_name);
                close(fd_myfs);
                return -1;
        }

        // Open destination file

        // Copy file data

        return 0;
}
