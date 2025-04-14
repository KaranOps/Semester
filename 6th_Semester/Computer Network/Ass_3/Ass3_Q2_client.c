#include<arpa/inet.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#define PORT 8080

int main(int argc, char const* argv[]){
	int status,valread,client_fd;
	struct sockaddr_in serv_addr;
	char *giveTime = "What's the time?";
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

	send(client_fd,giveTime,strlen(giveTime),0);
	printf("Asking for time\n");
	valread = read(client_fd,buffer,1024-1);
	printf("%s\n",buffer);

	close(client_fd);
	return 0;
}
