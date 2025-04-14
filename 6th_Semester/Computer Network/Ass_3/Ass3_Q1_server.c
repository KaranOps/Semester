#include<stdio.h>
#include<netinet/in.h> // has the sockaddr_in structure
#include<stdlib.h> //for the exit()
#include<string.h>
#include<sys/socket.h> //for using the different function of socket
#include<unistd.h> //for the close()

#define PORT 8080

int main(int argc,char const*argv[]){
	int server_fd, new_socket;
	ssize_t valread;
	struct sockaddr_in address;
	int opt = 1;
	int status;
	socklen_t addrlen = sizeof(address);
	char buffer[1024] = {0};
	// char *hello = "Hello from server";
		
	//Creating socket file descriptor
	server_fd = socket(AF_INET,SOCK_STREAM,0);
	if(server_fd<0){
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	//
	// int status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof
	// 		(opt));
	// if(status<0){
	// 	perror("setsockopt");
	// 	exit(EXIT_FAILURE);
	// }

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	//Binding
	status = bind(server_fd, (struct sockaddr*)&address,sizeof(address));
	if(status<0){
		perror("Binding failed");
		exit(EXIT_FAILURE);
	}
	
	//Listen
	status=listen(server_fd,3);
	if(status<0){
		perror("listen failed");
                exit(EXIT_FAILURE);

	}

	//Accept
	new_socket = accept(server_fd,(struct sockaddr*)&address,&addrlen);
	if(new_socket<0){
		perror("accept failed");
                exit(EXIT_FAILURE);

	}
	
	char stopMessage[3] = "Bye";
	
	printf("reading the message recived from client\n");
	char message[1024]={0};
	while(1){
		//clearing the buffer
		memset(buffer,0,sizeof(buffer));
	
		valread = read(new_socket, buffer, sizeof(buffer)-1);
		printf("Client:%s\n", buffer);

		if(strcmp(buffer,"Bye")==0){
			printf("closing chat\n");
			send(new_socket, "Bye", strlen("Bye"),0);	
			break;
		}

		printf("Server: ");
		fgets(message,sizeof(message),stdin);

		//remove the newline character
		message[strcspn(message,"\n")]=0;

		if(strcmp(message,"Bye")==0){
			send(new_socket, "Bye", strlen("Bye"),0);
                        break;
                }

		send(new_socket, message, strlen(message),0);
	}
	
	printf("After loop\n");
/*	valread = read(new_socket, buffer, 1024-1); //subtract 1 for the null at the end
	printf("%s\n", buffer);
	
	if(buffer==stopMessage){
		close(new_socket);//close the connected server
	        close(server_fd); //close the listening server

	}
	
	char message[1024]={0};
	scanf("%[^\n]s", message);

	if(message==stopMessage){
                close(new_socket);//close the connected server
                close(server_fd); //close the listening server

        }


	// printf("sending back the same message recived from client\n");
	send(new_socket, message, strlen(message),0);
	printf("Message send\n");
*/
	close(new_socket);//close the connected server
	close(server_fd); //close the listening server

	return 0;
}
