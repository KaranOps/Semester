#include<stdio.h>
#include<unistd.h>
#include<string.h>

//21 byte descriptor
struct byte_descriptor{
        char type;
        char name[12];
        int block_no; //block number of 1st block
        int size;
};
//Superblock structure
struct superblock{
        int size;
        int total_blocks;
        int block_no; //block number of 1st free block
        int root_block_no; //block no. of 1st block of the root folder
};

struct block{
        char data[4092]; //4096-4 bytes for data
        int nxt; //4bytes for next block number
};

void mymkfs(char *filename){
        FILE *fs = fopen(filename,"wb");
        if (fs == NULL)
        {
                 perror("creation of file failed");
                 return -1;
        }

        superblock sb;
        sb.total_blocks = 1000;
        sb.block_no = 2;
        sb.root_block_no = 1;


}
int main(){

        return 0;
}