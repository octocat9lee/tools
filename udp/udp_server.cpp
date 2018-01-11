#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define MAXDATASIZE 1400

void printSendQueue(int sockfd)
{
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name));
    if(-1 == ioctl(sockfd, SIOCGIFTXQLEN, &ifr))
    {
        printf("ioctl failed\n");
        exit(1);
    }
    printf("ifr_qlen %d\n", ifr.ifr_qlen);
}

int main()
{
    int sockfd;
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t addrlen;
    int num;
    char buf[MAXDATASIZE];

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("create socket failed\n");
        exit(1);
    }

    printSendQueue(sockfd);

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("bind error\n");
        exit(1);
    }

    addrlen = sizeof(client);
    while(true)
    {
        num = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr*)&client, &addrlen);

        if(num < 0)
        {
            perror("recvfrom() error\n");
            exit(1);
        }

        buf[num] = '\0';
        printf("receive from [%s] from client\n ip is [%s], port is %d\n", buf, inet_ntoa(client.sin_addr), htons(client.sin_port));
    }
    close(sockfd);
}
