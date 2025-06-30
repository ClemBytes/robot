#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "base64.h"

int handle_client(int clientfd, struct sockaddr_in client_addr, int* click_counter_ptr, char* favicon_data, char* html_template, char* css_template, size_t css_template_size) {
    // Get client IP address in '0.0.0.0' format for printing
    char client_ip_address[16];
    const char* ret2 = inet_ntop(AF_INET, &client_addr.sin_addr, client_ip_address, sizeof client_ip_address);
    if (ret2 == NULL) {
        perror("inet_ntop() failed");
        return 1;
    }
    printf("Client IP address: %s\n", client_ip_address);


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

        // Get first line of request
        int i;
        int first_line_size = 100;
        char first_line[first_line_size];
        for (i=0; i < n; i++) {
            if (buf[i] == '\r'){
                first_line[i] = '\r';
                first_line[i+1] = '\n';
                break;
            } else if (buf[i] == '\n') {
                first_line[i] = '\r';
                first_line[i+1] = '\n';
                break;
            } else {
                first_line[i] = buf[i];
            }
        }
        if (i >= first_line_size - 1) {
            printf("Size of first line is not enough: %d given and needs %d!\n", first_line_size, i);
            break;
        }
        first_line[i + 1] = 0;
        printf("Request first line: %s\n", first_line);

        // Parse first line
        char method[16], path[1024], version[16];
        sscanf(first_line, "%s %s %s", method, path, version);
        printf("Method: %s\n", method);
        printf("Path: %s\n", path);
        printf("Version: %s\n", version);
        printf("-------------------------------------\n");

        int content_length;
        char* content_type;
        char content[100000];

        // Generate response depending on request
        if (strcmp(method, "GET") == 0 && strcmp(path, "/data/template.css") == 0) {
            // Request for CSS file
            content_length = css_template_size;
            content_type = "text/css";
            int h = snprintf(content, sizeof content, "%s", css_template);
            if (h < 0) {
                perror("snprintf() for content failed");
                break;
            } else if (h >= sizeof content) {
                printf("Size of HTML header is not enough: %d given and needs %d!\n", sizeof content, h);
                break;
            }


        } else {
            // Other requests
            // Update button value
            if (strcmp(method, "POST") == 0 && strcmp(path, "/increment") == 0) {
                (*click_counter_ptr)++;
            } else if (strcmp(method, "POST") == 0 && strcmp(path, "/decrement") == 0) {
                (*click_counter_ptr)--;
            } else if (strcmp(method, "POST") == 0 && strcmp(path, "/reinitialize") == 0) {
                *click_counter_ptr = 0;
            }
            
            // Create HTML response
            content_type = "text/html";
            content_length = snprintf(content, sizeof content, html_template, favicon_data, clientfd, client_ip_address, *click_counter_ptr);
            if (content_length < 0) {
                perror("snprintf() for html failed");
                break;
            } else if (content_length >= sizeof content) {
                printf("Size of HTML response is not enough: %d given and needs %d!\n", sizeof content, content_length);
                break;
            }
        }

        // Create header for response
        int header_size = 100;
        char header[header_size];
        int h = snprintf(header, sizeof header, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", content_type, content_length);
        if (h < 0) {
            perror("snprintf() for header failed");
            break;
        } else if (h >= header_size) {
            printf("Size of HTML header is not enough: %d given and needs %d!\n", header_size, h);
            break;
        }

        // Concatenate header and data of response
        char str[h + content_length + 1];
        int ret = snprintf(str, sizeof str, "%s%s", header, content);
        if (ret < 0) {
            perror("snprintf() for concatenation failed");
            break;
        }

        // Send response to client (write to client file descriptor)
        ret = write(clientfd, str, ret);
        if (ret < 0) {
            perror("write() failed");
            break;
        }
    }
    return 0;
}


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

    // Initialize button
    int click_counter = 0;

    // Read favicon data:
    char* favicon_data = base64_from_path("./data/favicon-16x16.png", NULL);

    // Read HTML template:
    char* html_template = open_and_read("./data/template.html", NULL);

    // Read CSS template:
    size_t css_template_size;
    char* css_template = open_and_read("./data/template.css", &css_template_size);

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
        handle_client(clientfd, client_addr, &click_counter, favicon_data, html_template, css_template, css_template_size);
    }

    free(favicon_data);
    free(html_template);
    free(css_template);
}