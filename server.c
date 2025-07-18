#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>

#include "base64.h"
#include "string.h"

ssize_t read_client(int clientfd, char* buf, size_t* p_data_len, size_t* p_buf_size) {
    /*
    Reading of client buffer into buf at position data_len (data_len is the
    size of already written data) for as many characters as possible (to fill
    the buffer buf or to read everything).

    Returns an int:
        0: client disconnected
        -1: fail
        n: read response, number of characters read
    */
    ssize_t n = read(clientfd, buf + *p_data_len, *p_buf_size - *p_data_len);
    if (n == 0) {
        printf("Client %d disconnected\n", clientfd);
        printf("-------------------------------------\n");
        printf("-------------------------------------\n");
        return 0;
    }

    if (n < 0) {
        perror("read() failed");
        return -1;
    }

    *p_data_len += (size_t) n; // negative cases already dealt with

    if (*p_data_len == *p_buf_size) {
        // Buf is full, need to realloc
        *p_buf_size *= 2;
        buf = realloc(buf, *p_buf_size);
        // printf("REALLOC!! New size: %zu\n", buf_size);
    }
    return n;
}

void handle_client(int clientfd, struct sockaddr_in client_addr, int* x_coord, int* y_coord, char* favicon_data, char* html_template, char* css_template, size_t css_template_size, char* robot_png, size_t robot_png_size) {
    // Get client IP address in '0.0.0.0' format for printing
    char client_ip_address[16];
    const char* ret2 = inet_ntop(AF_INET, &client_addr.sin_addr, client_ip_address, sizeof client_ip_address);
    if (ret2 == NULL) {
        perror("inet_ntop() failed");
        return;
    }
    printf("Client IP address: %s\n", client_ip_address);

    // Read data sent from client
    size_t buf_size = 10;
    size_t data_len = 0;
    char* buf = malloc(buf_size);

    // Repeat indefinitely for each new request from current client
    while (1) {
        // Reinitialize data_len
        data_len = 0;

        struct pollfd client_pollfd;
        client_pollfd.fd = clientfd;
        client_pollfd.events = POLLIN; // can I read?

        ssize_t n = read_client(clientfd, buf, &data_len, &buf_size);
        if (n == 0) {
            return;
        }
        if (n < 0) {
            fprintf(stderr, "%s:%d - read_client() failed\n", __FILE__, __LINE__);
            free(buf);
            return;
        }

        while (1) {
            int r = poll(&client_pollfd, 1, 0); // timeout = 0 causes poll() to return immediately, even if no file descriptors are ready
            if (r < 0) {
                perror("poll() failed");
                return;
            }

            if ((client_pollfd.revents & POLLIN) == 0) {
                break;
            }

            // Fill buf from where we stopped
            n = read_client(clientfd, buf, &data_len, &buf_size);
            if (n == 0) {
                break;
            }
            if (n < 0) {
                fprintf(stderr, "%s:%d - read_client() failed\n", __FILE__, __LINE__);
                free(buf);
                break;
            }
        }
        // Finally, last realloc to have the exact needed size and add final 0
        buf = realloc(buf, data_len + 1);
        buf_size = data_len + 1;
        buf[data_len] = 0;

        // Get first line of request
        size_t i;
        struct string _first_line;
        struct string* first_line = &_first_line;
        string_init(first_line);
        for (i = 0; i < data_len; i++) {
            if (buf[i] == '\r'){
                string_append_with_size(first_line, "\r", 1);
                string_append_with_size(first_line, "\n", 1);
                break;
            } else if (buf[i] == '\n') {
                string_append_with_size(first_line, "\r", 1);
                string_append_with_size(first_line, "\n", 1);
                break;
            } else {
                string_append_with_size(first_line, &buf[i], 1);
            }
        }
        if (i >= first_line->used_size - 1) {
            fprintf(stderr, "Size of first line is not enough: %zu given and needs %zu!\n", first_line->used_size, i);
            break;
        }
        string_append_with_size(first_line, "\0", 1);

        // Parse first line
        char method[16], path[1024], version[16];
        int nb_match = sscanf(first_line->start, "%s %s %s", method, path, version);
        if (nb_match != 3) {
            fprintf(stderr, "Uncomplete request first line\n");
            continue;
        }
        printf("Method: %s\n", method);
        printf("Path: %s\n", path);
        printf("Version: %s\n", version);
        printf("-------------------------------------\n");

        // Init response content
        int content_length;
        char* content_type;
        char* content;

        // Generate response depending on request
        if (strcmp(method, "GET") == 0 && strcmp(path, "/data/template.css") == 0) {
            // Request for CSS file
            content_length = css_template_size;
            content_type = "text/css";
            content = malloc(content_length);
            memcpy(content, css_template, content_length);
        } else if (strcmp(method, "GET") == 0 && strcmp(path, "/data/robot.png") == 0) {
            // Request for robot PNG file
            content_length = robot_png_size;
            content_type = "image/png";
            content = malloc(content_length);
            memcpy(content, robot_png, content_length);
        } else {
            // Other requests
            // Update coordinates
            int x_max = 4;
            int y_max = 4;
            if (strcmp(method, "POST") == 0 && strcmp(path, "/down") == 0) {
                if ((*x_coord) == x_max) {
                    (*x_coord) = 0;
                } else {
                    (*x_coord)++;
                }
            } else if (strcmp(method, "POST") == 0 && strcmp(path, "/up") == 0) {
                if ((*x_coord) == 0) {
                    (*x_coord) = x_max;
                } else {
                    (*x_coord)--;
                }
            } else if (strcmp(method, "POST") == 0 && strcmp(path, "/right") == 0) {
                if ((*y_coord) == y_max) {
                    (*y_coord) = 0;
                } else {
                    (*y_coord)++;
                }
            } else if (strcmp(method, "POST") == 0 && strcmp(path, "/reset") == 0) {
                (*x_coord) = 0;
                (*y_coord) = 0;
            } else if (strcmp(method, "POST") == 0 && strcmp(path, "/left") == 0) {
                if ((*y_coord) == 0) {
                    (*y_coord) = y_max;
                } else {
                    (*y_coord)--;
                }
            } else if (strcmp(method, "GET") == 0 && strcmp(path, "/") == 0) {
                // Request for head page
                // Do nothing
                fprintf(stderr, "Request main page!\n");
            } else {
                fprintf(stderr, "Invalid request!\n");
                continue;
            }

            // Generate HTML table
            struct string _robot_grid;
            struct string* robot_grid = &_robot_grid;
            string_init(robot_grid);
            for (int x = 0; x < x_max + 1; x++) {
                string_append(robot_grid, "<tr class='tr-robot-grid'>");
                for (int y = 0; y < y_max + 1; y++) {
                    if ((x == *x_coord) && (y == *y_coord)) {
                        string_append(robot_grid, "<td class='td-robot-grid'><img src='data/robot.png' alt='Robot' class='image-responsive'></td>");
                    } else {
                        string_append(robot_grid, "<td class='td-robot-grid'></td>");
                    }
                }
                string_append(robot_grid, "</tr>");
            }
            
            // Create HTML response
            content_type = "text/html";
            content_length = snprintf(NULL, 0, html_template, favicon_data, *x_coord, *y_coord, robot_grid->start);
            if (content_length < 0) {
                perror("snprinft() for content_length failed");
                free(content);
                string_deinit(robot_grid);
                break;
            }
            content = malloc(content_length + 1);
            int h = snprintf(content, content_length + 1, html_template, favicon_data, *x_coord, *y_coord, robot_grid->start);
            if (h < 0) {
                perror("snprintf() for html failed");
                free(content);
                string_deinit(robot_grid);
                break;
            } else if (h >= content_length + 1) {
                printf(content);
                printf("%s:%d - Size of HTML response is not enough: %d given and needs %d!\n", __FILE__, __LINE__, h, content_length + 1);
                free(content);
                string_deinit(robot_grid);
                break;
            }
            string_deinit(robot_grid);
        }

        // Create header for response
        int header_size = snprintf(NULL, 0, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", content_type, content_length);
        if (header_size < 0) {
            perror("snprintf() for header_size failed");
            free(content);
            break;
        }
        char header[header_size + 1];
        int h = snprintf(header, sizeof header, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", content_type, content_length);
        if (h < 0) {
            perror("snprintf() for header failed");
            free(content);
            break;
        } else if (h >= (int)sizeof header) { // (int) to convert sizeof header which is a size_t into int for comparison
            printf("%s:%d - Size of HTML header is not enough: %d given and needs %zu!\n", __FILE__, __LINE__, h, sizeof header);
            free(content);
            break;
        }

        // Concatenate header and data of response
        char str[header_size + content_length];
        memcpy(str, header, (size_t) header_size);
        memcpy(str + header_size, content, content_length);

        free(content);

        // Send response to client (write to client file descriptor)
        ssize_t ret = write(clientfd, str, sizeof str);
        if (ret < 0) {
            perror("write() failed");
            break;
        }
    }
    return;
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
    int x_coord = 0;
    int y_coord = 0;

    // Read favicon data:
    char* favicon_data = base64_from_path("./data/favicon-16x16.png", NULL);

    // Read HTML template:
    char* html_template = open_and_read("./data/template.html", NULL);

    // Read CSS template:
    size_t css_template_size;
    char* css_template = open_and_read("./data/template.css", &css_template_size);

    // Read robot PNG:
    size_t robot_png_size;
    char* robot_png = open_and_read("./data/robot.png", &robot_png_size);

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
        handle_client(clientfd, client_addr, &x_coord, &y_coord, favicon_data, html_template, css_template, css_template_size, robot_png, robot_png_size);
    }

    free(favicon_data);
    free(html_template);
    free(css_template);
}