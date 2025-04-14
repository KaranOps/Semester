#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 49518  

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server is listening on port %d\n", PORT);

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    read(new_socket, buffer, sizeof(buffer));
    int query = atoi(buffer);
    printf("Client requested: %d\n", query);

    time_t t;
    struct tm *tm_info;
    char response[1024];

    switch (query) {
        case 1:
            time(&t);
            tm_info = localtime(&t);
            strftime(response, sizeof(response), "Time: %H:%M:%S", tm_info);
            break;
        case 2:
            time(&t);
            tm_info = localtime(&t);
            strftime(response, sizeof(response), "Date & Time: %Y-%m-%d %H:%M:%S", tm_info);
            break;
        case 3:
            gethostname(response, sizeof(response));
            break;
        default:
            strcpy(response, "Unsupported query");
            break;
    }

    send(new_socket, response, strlen(response), 0);
    printf("Sent response to client: %s\n", response);

    close(new_socket);
    close(server_fd);

    return 0;
}
