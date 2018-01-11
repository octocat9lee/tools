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

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2)
    {
        printf("usage: %s <IPaddress>\n", argv[0]);
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    char buff[MAXDATASIZE] = {0};
    for(int i = 0; i < MAXDATASIZE - 1; ++i)
    {
        buff[i] = i % 10 + '0';
    }

    while(true)
    {
        sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
        sleep(5);
    }

    close(sockfd);
    return 0;
}
