#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// packet structure
typedef struct
{
    char messageType;            // MT: 1 for payload packet, 2 for error
    unsigned int sequenceNumber; // SN: Unique identifier for each packet
    unsigned char ttl;           // TTL: Time-to-live value
    unsigned int payload_len;    // PL: Length of payload in bytes
    char payload[1000];          // PB: Actual payload data (max 1000 bytes)
} packet;

// Error packet structure
typedef struct
{
    char messageType;            // MT: 2 for error packet
    unsigned int sequenceNumber; // SN: Copied from the original packet
    char errorMessage[100];      // Error description
} errorPacket;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    packet packet;
    errorPacket errorPacket;
    int receivedBytes;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror(" Socket creation failed");
        exit(1);
    }
    printf("UDP server socket created\n");

    // Zero-initializes the entire sockaddr_in structure to avoid garbage values.
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to server address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror(" Bind failed");
        close(sockfd);
        exit(1);
    }
    printf(" Server bound to port %d\n", port);

    // Main server loop
    while (1)
    {
        addr_size = sizeof(client_addr);

        // Receive packet from client
        receivedBytes = recvfrom(sockfd, &packet, sizeof(packet), 0,
                                 (struct sockaddr *)&client_addr, &addr_size);

        if (receivedBytes < 0)
        {
            perror(" Receive failed");
            continue;
        }

        printf(" packet received from client. SN: %u, TTL: %d, PL: %u\n",
               packet.sequenceNumber, packet.ttl, packet.payload_len);

        // Validate packet
        if (packet.messageType != 1)
        {
            // Prepare error packet - invalid message type
            errorPacket.messageType = 2;
            errorPacket.sequenceNumber = packet.sequenceNumber;
            strcpy(errorPacket.errorMessage, "Invalid message type. Expected 1.");

            sendto(sockfd, &errorPacket, sizeof(errorPacket), 0,
                   (struct sockaddr *)&client_addr, addr_size);
            continue;
        }

        // Validate payload length
        if (packet.payload_len < 100 || packet.payload_len > 1000)
        {
            // Prepare error packet - invalid payload length
            errorPacket.messageType = 2;
            errorPacket.sequenceNumber = packet.sequenceNumber;
            strcpy(errorPacket.errorMessage, "Invalid payload length. Expected 100-1000 bytes.");

            sendto(sockfd, &errorPacket, sizeof(errorPacket), 0,
                   (struct sockaddr *)&client_addr, addr_size);
            continue;
        }

        // Validate TTL
        if (packet.ttl <= 0)
        {
            // Prepare error packet - TTL expired
            errorPacket.messageType = 2;
            errorPacket.sequenceNumber = packet.sequenceNumber;
            strcpy(errorPacket.errorMessage, "TTL expired.");

            sendto(sockfd, &errorPacket, sizeof(errorPacket), 0,
                   (struct sockaddr *)&client_addr, addr_size);
            continue;
        }

        // Process valid packet - decrease TTL by 1
        packet.ttl--;

        // Send packet back to client
        if (sendto(sockfd, &packet, sizeof(packet), 0,
                   (struct sockaddr *)&client_addr, addr_size) < 0)
        {
            perror(" Send failed");
        }
        else
        {
            printf(" packet forwarded back to client. SN: %u, TTL: %d\n",
                   packet.sequenceNumber, packet.ttl);
        }
    }

    close(sockfd);
    return 0;
}
