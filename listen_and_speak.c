#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket created, sockfd: %d\n", sockfd);
    // A socket is like a case number (a file descriptor)
    // Here, we tell him that we wanna use the IPV4 protocol family (AF_INET),
    // and the TCP protocol (SOCK_STREAM)

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval);
    // I always want this to avoid error when I use this twice in a row

    /*
    struct sockaddr_in {
        short AF_INET;
        unsigned short htons(8000);
        struct in_addr sin_addr;   // see struct in_addr, below
        char sin_zero[8]; // zero this if you want to
    };
    */

    struct in_addr mysinaddr;
    inet_aton("0.0.0.0", &mysinaddr);

    struct sockaddr_in myaddr = {
        .sin_family = AF_INET,
        .sin_port = htons(8000),
        .sin_addr = mysinaddr,
    };

    bind(sockfd, (struct sockaddr*) &myaddr, sizeof myaddr);

    listen(sockfd, 1);

    int clientfd = accept(sockfd, NULL, NULL);
    printf("Connexion received, clientfd: %d\n", clientfd);

    char buf[1000];
    ssize_t n = read(clientfd, buf, sizeof buf);
    printf("Data received, size: %zi\n", n);
    printf("Data: %s\n", buf);
    
    write(clientfd, buf, n);
}