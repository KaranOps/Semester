#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int server_fd, new_socket;
 int running = 1;

void* receive_messages(void* arg) {
    char buffer[BUFFER_SIZE];
     while (running) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) break;

        printf("\nClient: %s", buffer);
        if (strncmp(buffer, "exit", 4) == 0) {
            running = 0;
            break;
        }
    }
    return NULL;
}

void* send_messages(void* arg) {
    char buffer[BUFFER_SIZE];
    while (running) {
        fgets(buffer, BUFFER_SIZE, stdin);
        send(new_socket, buffer, strlen(buffer), 0);
        if (strncmp(buffer, "exit", 4) == 0) {
            running = 0;
            break;
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);
    
    printf("Server is listening on port %d...\n", PORT);
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    printf("Client connected!\n");

    pthread_t send_thread, recv_thread;
    pthread_create(&send_thread, NULL, send_messages, NULL);
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    close(new_socket);
    close(server_fd);
    return 0;
}
