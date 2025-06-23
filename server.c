#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    /*
    Create a socket and get its file descriptor.
    
    A socket is like a case number (a file descriptor)
    Here, we tell him that we wanna use the IPV4 protocol family (AF_INET),
    and the TCP protocol (SOCK_STREAM)
    */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket() failed");
        return 1;
    }
    printf("Socket created, sockfd: %d\n", sockfd);

    // I always want this to avoid error when I use this twice in a row
    int optval = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval);
    if (ret < 0) {
        perror("setsockopt() failed");
        return 1;
    }

    // Define IP address and port
    const char *interface = "0.0.0.0";
    struct in_addr mysinaddr;
    ret = inet_aton(interface, &mysinaddr);
    if (ret == 0) {
        fprintf(stderr, "Invalid IP address: %s\n", interface);
        return 1;
    }

    struct sockaddr_in myaddr = {
        .sin_family = AF_INET,
        .sin_port = htons(8000),
        .sin_addr = mysinaddr,
    };

    // Attach socket to the previously defined address and port
    ret = bind(sockfd, (struct sockaddr*) &myaddr, sizeof myaddr);
    if (ret < 0) {
        perror("bind() failed");
        return 1;
    }

    // Mark the socket as ready go receive entry connexions
    ret = listen(sockfd, 1);
    if (ret < 0) {
        perror("listen() failed");
        return 1;
    }

    // Repeat indefinitely for each new client
    while (1) {
        // Get client connexion address
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof client_addr;
        int clientfd = accept(sockfd, (struct sockaddr*) &client_addr, &client_addr_len);
        if (clientfd < 0) {
            perror("accept() failed");
            return 1;
        }
        printf("\n --- NEW CONNEXION RECEIVED, clientfd: %d ---\n", clientfd);
        // Get client IP address in '0.0.0.0' format for printing
        char dst[16];
        const char* ret2 = inet_ntop(AF_INET, &client_addr.sin_addr, dst, sizeof dst);
        if (ret2 == NULL) {
            perror("inet_ntop() failed");
            return 1;
        }
        printf("Client IP address: %s\n", dst);


        char buf[1000];
        // Repeat indefinitely for each new request from current client
        while (1) {
            // Read data sent from client
            // -1 to keep last bit for 0
            ssize_t n = read(clientfd, buf, (sizeof buf) - 1);
            if (n == 0) {
                printf("Client %d disconnected\n", clientfd);
                printf("-------------------------------------\n");
                printf("-------------------------------------\n");
                break;
            } else if (n < 0) {
                perror("read() failed");
                break;
            }
            buf[n] = 0;
            printf("Data received, size: %zi\n", n);
            printf("DATA:\n");
            printf("-------------------------------------\n");
            printf("%s\n", buf);
            printf("-------------------------------------\n");

            // Create header for response
            char header[100];
            int w = snprintf(header, sizeof header, "HTTP/1.0 200 OK\r\nContent-Length: %d\r\n\r\n", n);
            if (w < 0) {
                perror("snprintf() for header failed");
                break;
            }

            // Concatenate header and data of response
            char str[w + n + 1];
            ret = snprintf(str, sizeof str, "%s%s", header, buf);
            if (ret < 0) {
                perror("snprintf() for concatenation failed");
                break;
            }

            // Send response to client (write to client file descriptor)
            ret = write(clientfd, str, w + n);
            if (ret < 0) {
                perror("write() failed");
                break;
            }
        }
    }
}