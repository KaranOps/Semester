#include <stdio.h>
#include<sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


typedef struct {
        uint8_t messType;
        uint16_t sequenceNo;
        uint8_t ttl;
        uint32_t payloadLength;
        char payload[1000];


}packet;

typedef struct {
        uint8_t messageType;
        uint16_t sequenceNo;
        uint8_t EC;
}errpacket;


double time_diff(struct timeval *start, struct timeval *end) {
        return (end->tv_sec - start->tv_sec) * 1000.0 + (end->tv_usec - start->tv_usec) / 1000.0;
}


void print_error_message(uint8_t error_code) {
        switch (error_code) {
                case 1:
                        printf("Error: TOO SMALL PACKET RECEIVED\n");
                        break;
                case 2:
                        printf("Error: PAYLOAD LENGTH AND PAYLOAD INCONSISTENT\n");
                        break;
                    case 3:
                        printf("Error: TOO LARGE PAYLOAD LENGTH\n");
                        break;
                case 4:
                        printf("Error: TTL VALUE IS NOT EVEN\n");
                        break;
                default:
                        printf("Error: UNKNOWN ERROR CODE\n");
                        break;
        }
}

int main(int argc, char* argv[]){

        if(argc!= 6){
                printf("incorrect arguments\n");
                exit(1);
        }

        char *serverIP = argv[1];
        int serverPort = atoi(argv[2]);
        int payloadSize = atoi(argv[3]);
        int ttl = atoi(argv[4]);
        int numPackets= atoi(argv[5]);
        if(payloadSize < 100 || payloadSize > 1000){
                perror("Messagesize is not appropriate\n");
                exit(1);
        }
        if(numPackets < 1 || numPackets > 50){
                perror("packets numbers is not appropriate\n");
                exit(1);
        }
        if(ttl%2!= 0 || ttl < 2 || ttl > 20){
                perror("ttl is wrong\n");
                exit(1);
        }

        int sockfd;
        struct sockaddr_in server_addr;
        socklen_t addr_len = sizeof(server_addr);

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
                perror("Socket creation failed");
                return 1;


        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(serverPort);
        if (inet_pton(AF_INET, serverIP, &server_addr.sin_addr) <= 0) {
                perror("Invalid address");
                return 1;
        }

        double totalRTT = 0.0;

        for(int i = 0; i < numPackets; i++){
                packet pkt;
                pkt.messType = 1;
                pkt.sequenceNo = htons(i);
                pkt.ttl = ttl;
                pkt.payloadLength = payloadSize;

                memset(pkt.payload, 'K', payloadSize);


                struct timeval start, end;
                gettimeofday(&start, NULL);

                if (sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr*)&server_addr, addr_len) < 0) {
                        perror("failed to send packet\n");
                        continue;
                }
                char buffer[1000];


                ssize_t recvLen = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &addr_len);
                gettimeofday(&end, NULL);

                if(buffer[0] == 2){

                        printf("Erroreous packet: sequenceNo = %d", i);
                        print_error_message(buffer[3]);

                }
                else{
                        double rtt = time_diff(&start,&end);
                        totalRTT += rtt;
                        printf("Packet %d RTT: %.3f ms\n", i, rtt);
                }



        }
        printf("Average RTT: %.3f ms\n", totalRTT / numPackets);
        close(sockfd);
        return 0;



}

