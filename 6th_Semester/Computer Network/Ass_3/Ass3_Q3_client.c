#include<arpa/inet.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<ctype.h> //for isdigit()

#define PORT 8080

void stringToArray(char *s,int* arr){
        int len = strlen(s);
        int idx=0;
        int num=0;
        int isNum = 0;

        for(int i=0;i<len;i++){
                if(s[i]!=','){
                        if(isdigit(s[i])){
                                num = num*10 + (s[i]-'0');
                                isNum = 1;
                        }
                }else{
                        if(isNum){
                                arr[idx++] = num;
                                num=0;
                                isNum = 0;
                        }
                }
        }

        if(isNum){
                arr[idx++] = num;
        }
}

void ArrayToString(int *arr,int n,char *s){
        int idx = 0;
        for(int i=0;i<n;i++){
                idx += sprintf(s+idx,"%d",arr[i]);
                if(i<n-1){
                        s[idx++] = ',';
                }
        }
        s[idx] = '\0';
}


int main(int argc, char const* argv[]){
        int status,valread,client_fd;
        struct sockaddr_in serv_addr;
        
        char buffer[1024] = {0};
        client_fd = socket(AF_INET,SOCK_STREAM,0);
        if(client_fd<0){
                perror("socket creation failed\n");
                return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        status = inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);
        if(status<=0){
                printf("Invalid address\n");
                return -1;
        }

        status = connect(client_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
        if(status<0){
                perror("Connextion failed\n");
                return -1;
        }
	
	int size = 15;
	int arr[15] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};

	printf("Sending size of array\n");
	send(client_fd,&size,sizeof(size),0);
	printf("size send successfully\n");

	send(client_fd,arr,sizeof(arr),0);
	printf("Array send sucessfully\n");

	memset(arr,0,sizeof(arr));
	valread = read(client_fd,arr,sizeof(arr));
	printf("Recived the sorted array\n");

	printf("Sorted Array: ");
        //print the array
        for(int i=0;i<15;i++){
                printf("%d ",arr[i]);
        }



/*	char sendBuffer[1024]={0};
	sprintf(sendBuffer,"%d",size);
	
	//Sending size
	printf("Sending size of array\n");
	send(client_fd,sendBuffer,strlen(sendBuffer),0);
	printf("size send successfully\n");

	//Converting array into string for sending
	ArrayToString(arr,size,sendBuffer);
	
	
	//send the string
	send(client_fd,sendBuffer,strlen(sendBuffer),0);
	printf("Array send sucessfully\n");

	//get the sorted string
	valread = read(client_fd,buffer,1024-1);
	printf("Recived the sorted array\n");

	//convert it into array
	int arr2[15];
	stringToArray(buffer, arr2);
	
	printf("Sorted Array: ");
	//print the array
	for(int i=0;i<15;i++){
		printf("%d ",arr2[i]);
	}
*/	
	printf("\n");




	close(client_fd);
        return 0;
}

