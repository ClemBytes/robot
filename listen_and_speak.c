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

    struct sockaddr_in client_adddr;
    socklen_t client_addr_len = sizeof client_adddr;
    int clientfd = accept(sockfd, (struct sockaddr*) &client_adddr, &client_addr_len);
    printf("Connexion received, clientfd: %d\n", clientfd);
    char dst[16];
    inet_ntop(AF_INET, &client_adddr.sin_addr, dst, sizeof dst);
    printf("%s\n", dst);


    char buf[1000];
    /*
    printf("Contents of buffer: ");
    for (size_t i = 0; i < 20; i++) {
        printf("%d, ", buf[i]);
    }
    printf("…\n");
    */
    while (1) {
        ssize_t n = read(clientfd, buf, (sizeof buf) - 1);
        // -1 to keep last bit for 0
        buf[n] = 0;
        printf("Data received, size: %zi\n", n);
        /*
        printf("Contents of buffer: ");
        for (size_t i = 0; i < 20; i++) {
            printf("%d, ", buf[i]);
        }
        printf("…\n");
        */
        printf("DATA:\n");
        printf("-------------------------------------\n");
        printf("%s\n", buf);
        printf("-------------------------------------\n");

        char header[100];
        int w = snprintf(header, sizeof header, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\n\r\n", n);
        write(clientfd, header, w);
        write(clientfd, buf, n);
    }
}