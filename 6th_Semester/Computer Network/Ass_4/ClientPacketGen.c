#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

// packet structure
typedef struct
{
    char messageType;
    unsigned int sequenceNumber; // because "unsigned int" is of 2 bytes
    char ttl;
    int payload_len;
    char patload[1000];
} packet;

// Error packet structure
typedef struct
{
    char messageType;
    unsigned int sequenceNumber;
    char errorMessage[100];
} errorPacket;

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        printf("Too Many/Few command line arguments");
        exit(1);
    }

    char *serverIP = argv[1];
    int serverPort = atoi(argv[2]);
    int payloadSize = atoi(argv[3]);
    int ttl = atoi(argv[4]);
    int no_of_packets = atoi(argv[5]);

    // checking input parameters are valid or not
    if (payloadSize < 100 || payloadSize > 1000)
    {
        printf("Error: Payload size must be b/w 100 and 1000\n");
        exit(1);
    }

    if (ttl < 2 || ttl > 20 || ttl % 2 != 0)
    {
        printf("Error: TTL must be even number\n");
        exit(1);
    }

    if (no_of_packets < 1 || no_of_packets > 50)
    {
        printf("Error: number of packets must be b/w 1 and 50\n");
        exit(1);
    }

    int fd;
    struct sockaddr_in server_addr;
    socklen_t addr_size;
    packet paket;
    errorPacket err_packet;
    struct timeval startTime, endTime;
    double rttValues[no_of_packets];
    double totalRTT = 0.0;
    int receivedPacket = 0;

    // creating UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket creation failed");
        exit(1);
    }

    printf("UDP client socket created successfully\n");

    // Zero-initializes the entire sockaddr_in structure to avoid garbage values.
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(serverPort);
    server_addr.sin_addr.s_addr = inet_addr(serverIP);

    // setting socket timeout to be 5 seconds
    struct timeval tyme;
    tyme.tv_sec = 5;  // tv_sec: Seconds
    tyme.tv_usec = 0; // tv_usec: Microseconds
    if (setsockopt(fd, SOL_SOCKET, SO_REVTIMEO, &tyme, sizeof(tyme)) < 0)
    {
        perror("error in setting socket timeout");
        close(fd);
        exit(1);
    }

    printf("Sending %d packets to %s:%d with payload size %d bytes and TTL %d\n",
           no_of_packets, serverIP, serverPort, payloadSize, ttl);

    // sending packets and mesauring rtt
    for (int i = 0; i < no_of_packets; i++)
    {
        paket.messageType = 1;
        paket.sequenceNumber = i;
        paket.ttl = ttl;
        paket.payload_len = payloadSize;

        // filling payload with 'A'
        for (int j = 0; j < payloadSize; j++)
        {
            paket.patload[j] = 'A';
        }

        addr_size = sizeof(server_addr);

        // recording start time
        gettimeofday(&startTime, NULL);

        // sending packet to server
        if (sendto(fd, &paket, sizeof(packet), 0,
                   (struct sockaddr *)&server_addr, addr_size) < 0)
        {
            perror("Send failed");
            continue;
        }

        printf("packet %d send.SN: %u, TTL: %d, PL: %u\n",
               i, paket.sequenceNumber, paket.ttl, paket.payload_len);

        int receivedBytes = recvfrom(fd, &paket, sizeof(Packet), 0,
                                     (struct sockaddr *)&server_addr, &addr_size);

        // record the end time
        gettimeofday(&endTime, NULL);

        if (receivedBytes < 0)
        {
            perror("Recive failed or timeout");
            printf("Packet %d: No response (timeout)\n", i);
            continue;
        }

        // agar error packet hua toh
        if (paket.messageType == 2)
        {
            errorPacket *err = (errorPacket *)&paket;
            printf("Error for packet %d: %s\n", err->sequenceNumber, err->errorMessage);
            continue;
        }

        // calculating rtt in milliseconds
        double rtt = (endTime.tv_sec - startTime.tv_sec) * 1000.0;
        rtt += (endTime.tv_usec - startTime.tv_usec) / 1000.0;

        rttValues[receivedPacket] = rtt;
        totalRTT += rtt;
        receivedPacket++;

        printf("Packet %d: RTT = %.3f ms, TTL=%d\n", i, rtt, paket.ttl);

        // Small delay b/w packets
        usleep(100000); // 100ms
    }

    if (receivedPacket > 0)
    {
        double avgRtt = totalRTT / receivedPacket;
        printf("\nSummary:\n");
        printf("Packets sent: %d\n", no_of_packets);
        printf("Packets received: %d\n", receivedPacket);
        printf("Packet loss: %.2f%%\n", 100.0 * (no_of_packets - receivedPacket) / no_of_packets);
        printf("Average RTT: %.3f ms\n", avgRtt);

        // calculating min and max RTT
        double minRTT = rttValues[0];
        double maxRTT = rttValues[0];

        for (int i = 1; i < receivedPacket; i++)
        {
            if (rttValues[i] < minRTT)
                minRTT = rttValues[i];
            if (rttValues[i] > axT)
                maxT = rttValues[i];
        }

        printf("Minimum RTT: %.3f ms\n", minRTT);
        printf("Maximum RTT:  %.3f ms\n", maxRTT);
    }
    else
    {
        printf("\nNo packets were sucessfully received.\n");
    }

    clost(fd);
    return 0;
}