#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
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

void insertionSort(int *arr,int n){
	for(int i=1;i<n;i++){
		int curr = arr[i];
		int j = i-1;

		while(j>=0 && arr[j]>curr){
			arr[j+1] = arr[j];
			j = j-1;
		}
		arr[j+1] = curr;
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

int main(int argc,char const* argv[]){
	int server_fd,client_fd;
	ssize_t valread;
	struct sockaddr_in address;
	int opt = 1;
	socklen_t addrlen = sizeof(address);
	char buffer[1024] = {0};

	server_fd = socket(AF_INET,SOCK_STREAM,0);
	if(server_fd<0){
		perror("Scoket creation failed");
		exit(EXIT_FAILURE);
	}

	int status = setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        if(status<0){
                perror("setsockopt failed\n");
                exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);


        status = bind(server_fd, (struct sockaddr*)&address,sizeof(address));
        if(status<0){
                perror("Bind failes\n");
                exit(EXIT_FAILURE);
        }

        status = listen(server_fd,3);
        if(status<0){
                perror("listen failed\n");
		exit(EXIT_FAILURE);
        }

        client_fd = accept(server_fd,(struct sockaddr*)&address,&addrlen);
        if(client_fd<0){
                perror("accept failed\n");
                exit(EXIT_FAILURE);
        }

	int size;
	//Receving the size
	printf("Getting length from client\n");
	valread = read(client_fd,&size,sizeof(size));
	printf("Length of array: %d\n", size);

	int arr[15];
	printf("Getting array from client\n");
	valread = read(client_fd,arr,sizeof(arr));
	printf("Given array: ");
	for(int i=0;i<15;i++){
		printf("%d ",arr[i]);
	}
	
	printf("\n");

	//Sorting the array using inertion sort
        insertionSort(arr,size);

	send(client_fd,arr,valread,0);
	printf("Sorted Array Sent Successfully");

/*	printf("Getting length from client\n");
	valread = read(client_fd,buffer,1024-1);
	int size = atoi(buffer);
	printf("Length of array: %d\n", size);

	//Getting the array
	printf("Getting array from client\n");
	valread = read(client_fd,buffer,1024-1);

	printf("Given array: ");
	printf("%s", buffer);

	int arr[size];
	stringToArray(buffer,arr);
	printf("\n");

	//Sorting the array using inertion sort
	insertionSort(arr,size);
	char s[1024];
	ArrayToString(arr,size,s);


	//sending the string 
	send(client_fd, s, strlen(s),0);
	printf("Sorted Array Sent Successfully");
	
*/	printf("\n");
	

	close(client_fd);
        close(server_fd);

	return 0;
}
