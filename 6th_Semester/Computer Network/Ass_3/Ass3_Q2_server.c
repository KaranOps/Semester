#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<time.h>
#define PORT 8080

int main(int argc,char const* argv[]){
	int server_fd,client_fd;
	ssize_t valread;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);
	int opt = 1;
	char buffer[1024]={0};

	server_fd = socket(AF_INET,SOCK_STREAM,0);
	if(server_fd<0){
		perror("Socket creation failed\n");
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

	valread = read(client_fd,buffer,1024-1);
	printf("%s\n", buffer);
	
	time_t mytime = time(NULL);
	char *message = ctime(&mytime);
	//remove the extra line at the end of the string
	message[strlen(message)-1] = '\0';

	send(client_fd,message,strlen(message),0);
	printf("Time Send\n");

	close(client_fd);
	close(server_fd);

	return 0;
}
