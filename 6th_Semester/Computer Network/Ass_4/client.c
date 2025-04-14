#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

// Packet structure
typedef struct
{
    unsigned char mt;            // MT: 1 for payload packet, 2 for error
    unsigned int sn;             // SN: Unique identifier for each packet
    unsigned char ttl;           // TTL: Time-to-live value
    unsigned int payload_length; // PL: Length of payload in bytes
    char payload[1000];          // PB: Actual payload data (max 1000 bytes)
} Packet;

// Error packet structure
typedef struct
{
    unsigned char mt;        // MT: 2 for error packet
    unsigned int sn;         // SN: Copied from the original packet
    char error_message[100]; // Error description
} ErrorPacket;

int main(int argc, char **argv)
{
    if (argc != 6)
    {
        printf("Usage: %s <ServerIP> <ServerPort> <PayloadSize> <TTL> <NumPackets>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int payload_size = atoi(argv[3]);
    int ttl = atoi(argv[4]);
    int num_packets = atoi(argv[5]);

    // Validate input parameters
    if (payload_size < 100 || payload_size > 1000)
    {
        printf("Error: Payload size must be between 100 and 1000 bytes\n");
        exit(1);
    }

    if (ttl < 2 || ttl > 20 || ttl % 2 != 0)
    {
        printf("Error: TTL must be an even number between 2 and 20\n");
        exit(1);
    }

    if (num_packets < 1 || num_packets > 50)
    {
        printf("Error: Number of packets must be between 1 and 50\n");
        exit(1);
    }

    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t addr_size;
    Packet packet;
    ErrorPacket error_packet;
    struct timeval start_time, end_time;
    double rtt_values[num_packets];
    double total_rtt = 0.0;
    int received_packets = 0;
    int i, j;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("[-] Socket creation failed");
        exit(1);
    }
    printf("[+] UDP client socket created\n");

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Set socket timeout (5 seconds)
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("[-] Error setting socket timeout");
        close(sockfd);
        exit(1);
    }

    printf("Sending %d packets to %s:%d with payload size %d bytes and TTL %d\n",
           num_packets, server_ip, server_port, payload_size, ttl);

    // Send packets and measure RTT
    for (i = 0; i < num_packets; i++)
    {
        // Prepare packet
        packet.mt = 1;
        packet.sn = i;
        packet.ttl = ttl;
        packet.payload_length = payload_size;

        // Fill payload with a pattern (for this example, just filling with 'A's)
        for (j = 0; j < payload_size; j++)
        {
            packet.payload[j] = 'A';
        }

        addr_size = sizeof(server_addr);

        // Record start time
        gettimeofday(&start_time, NULL);

        // Send packet to server
        if (sendto(sockfd, &packet, sizeof(Packet), 0,
                   (struct sockaddr *)&server_addr, addr_size) < 0)
        {
            perror("[-] Send failed");
            continue;
        }

        printf("[+] Packet %d sent. SN: %u, TTL: %d, PL: %u\n",
               i, packet.sn, packet.ttl, packet.payload_length);

        // Receive response from server
        int received_bytes = recvfrom(sockfd, &packet, sizeof(Packet), 0,
                                      (struct sockaddr *)&server_addr, &addr_size);

        // Record end time
        gettimeofday(&end_time, NULL);

        if (received_bytes < 0)
        {
            perror("[-] Receive failed or timed out");
            printf("Packet %d: No response (timeout)\n", i);
            continue;
        }

        // Check if it's an error packet
        if (packet.mt == 2)
        {
            // Cast to error packet structure
            ErrorPacket *err = (ErrorPacket *)&packet;
            printf("[-] Error for packet %d: %s\n", err->sn, err->error_message);
            continue;
        }

        // Calculate RTT in milliseconds
        double rtt = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
        rtt += (end_time.tv_usec - start_time.tv_usec) / 1000.0;

        rtt_values[received_packets] = rtt;
        total_rtt += rtt;
        received_packets++;

        printf("Packet %d: RTT = %.3f ms, TTL = %d\n", i, rtt, packet.ttl);

        // Small delay between packets
        usleep(100000); // 100ms
    }

    // Calculate and display statistics
    if (received_packets > 0)
    {
        double avg_rtt = total_rtt / received_packets;
        printf("\nSummary:\n");
        printf("Packets sent: %d\n", num_packets);
        printf("Packets received: %d\n", received_packets);
        printf("Packet loss: %.2f%%\n", 100.0 * (num_packets - received_packets) / num_packets);
        printf("Average RTT: %.3f ms\n", avg_rtt);

        // Calculate min and max RTT
        double min_rtt = rtt_values[0];
        double max_rtt = rtt_values[0];
        for (i = 1; i < received_packets; i++)
        {
            if (rtt_values[i] < min_rtt)
                min_rtt = rtt_values[i];
            if (rtt_values[i] > max_rtt)
                max_rtt = rtt_values[i];
        }
        printf("Minimum RTT: %.3f ms\n", min_rtt);
        printf("Maximum RTT: %.3f ms\n", max_rtt);
    }
    else
    {
        printf("\nNo packets were successfully received.\n");
    }

    close(sockfd);
    return 0;
}
