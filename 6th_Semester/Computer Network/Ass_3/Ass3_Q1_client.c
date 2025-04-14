#include <arpa/inet.h> //for converting textual to binary formats.Here converts IPv4/IPv6 address from textual to binary
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(){
	int status, valread, client_fd;
	struct sockaddr_in serv_addr;
	// struct sockaddr_in {
	// 	short           sin_family;   // Address family (AF_INET for IPv4)
	// 	unsigned short  sin_port;     // Port number (use htons())
	// 	struct in_addr  sin_addr;     // IP address (use inet_addr())
	// 	char            sin_zero[8];  // Padding (not used, just fill with 0)
	// };
	
        //	char* hello = "Hello World";
        char buffer[1024] = { 0 };
	

	//Socket creation
	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(client_fd<0){
		printf("\n Socket creation error \n");
                return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	
	//Converting address from text to binary
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}
	
	//Connection
	status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if(status<0){
		printf("\nConnection Failed \n");
		return -1;
	}
	//Start the chat
	printf("Start chatting\n");
	char stopMessage[3] = "Bye";
	
	char message[1024] = {0};

	while(1){
		printf("Client: ");
		fgets(message,sizeof(message),stdin);
		message[strcspn(message,"\n")]=0;
		
		if(strcmp(message,"Bye")==0){
                        send(client_fd, "Bye", strlen("Bye"),0);
                        break;
                }


		send(client_fd,message,strlen(message),0);

		//clearing the buffer
		memset(buffer,0,sizeof(buffer));

		valread = read(client_fd, buffer, sizeof(buffer)-1);
                printf("Server:%s\n", buffer);
               
	       	if(strcmp(buffer,"Bye")==0){
                        printf("closing chat\n");
                        send(client_fd, "Bye", strlen("Bye"),0);
                        break;
                }
               // printf("Client: ");
		
	}
/*	scanf("%[^\n]s", message);
	send(client_fd,message,strlen(message),0);
        //	printf("Hello World sent from client side\n");
        //	printf("waiting for server to send message\n");
	valread = read(client_fd, buffer,1024-1);
	
	if(message==stopMessage || buffer==stopMessage){
		printf("close the client\n");
		close(client_fd);
	}
	printf("%s\n", buffer);
*///	printf("close the client\n");
	close(client_fd);

	return 0;
}
