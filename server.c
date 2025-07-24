#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>

#include "lib_server.h"

ssize_t read_client(int clientfd, char** p_buf, size_t* p_data_len, size_t* p_buf_size) {
    /*
    Reading of client buffer into buf at position data_len (data_len is the
    size of already written data) for as many characters as possible (to fill
    the buffer buf or to read everything).

    Returns an int:
        0: client disconnected
        -1: fail
        n: read response, number of characters read
    */
    ssize_t n = read(clientfd, *p_buf + *p_data_len, *p_buf_size - *p_data_len);
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
        *p_buf = realloc(*p_buf, *p_buf_size);
        // printf("REALLOC!! New size: %zu\n", buf_size);
    }
    return n;
}

void handle_client(int clientfd, struct sockaddr_in client_addr, struct templates* p_tem) {
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
    size_t data_len;
    char* buf = malloc(buf_size);

    // Init strings for response
    struct string _content;
    struct string* content = &_content;
    string_init(content);
    struct string _cookie;
    struct string* cookie = &_cookie;
    string_init(cookie);
    struct string _robot_grid;
    struct string* robot_grid = &_robot_grid;
    string_init(robot_grid);
    struct string _header;
    struct string* header = &_header;
    string_init(header);

    // Repeat indefinitely for each new request from current client
    while (1) {
        // Reinitialize data_len
        data_len = 0;

        struct pollfd client_pollfd;
        client_pollfd.fd = clientfd;
        client_pollfd.events = POLLIN; // can I read?

        ssize_t n = read_client(clientfd, &buf, &data_len, &buf_size);
        if (n == 0) {
            string_deinit(cookie);
            string_deinit(content);
            string_deinit(header);
            string_deinit(robot_grid);
            return;
        }
        if (n < 0) {
            fprintf(stderr, "%s:%d - read_client() failed\n", __FILE__, __LINE__);
            free(buf);
            string_deinit(cookie);
            string_deinit(content);
            string_deinit(header);
            string_deinit(robot_grid);
            return;
        }

        while (1) {
            int r = poll(&client_pollfd, 1, 0); // timeout = 0 causes poll() to return immediately, even if no file descriptors are ready
            if (r < 0) {
                perror("poll() failed");
                string_deinit(cookie);
                string_deinit(content);
                string_deinit(header);
                string_deinit(robot_grid);
                return;
            }

            if ((client_pollfd.revents & POLLIN) == 0) {
                break;
            }

            // Fill buf from where we stopped
            n = read_client(clientfd, &buf, &data_len, &buf_size);
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
        void *tmp = realloc(buf, data_len + 1);
        if (tmp == NULL) {
            free(buf);
            fprintf(stderr, "%s:%d - realloc() failed\n", __FILE__, __LINE__);
            break;
        } else {
            buf = tmp;
        }
        buf_size = data_len + 1;
        buf[data_len] = 0;

        // Parse client's request
        char method[16], path[1024], version[16];
        int cookie_x, cookie_y, cookie_found;
        parse_client_request(buf, data_len, method, path, version, &cookie_x, &cookie_y, &cookie_found);

        // Init x_coord and y_coord
        int x_coord, y_coord;
        if (cookie_found) {
            // init to cookie values
            x_coord = cookie_x;
            y_coord = cookie_y;
        } else {
            // No previous values, init to 0
            x_coord = 0;
            y_coord = 0;
        }
        // fprintf(stderr, "Coords: x=%d, y=%d\n", x_coord, y_coord);
        // fprintf(stderr, "\nClient's request:\n%s\n", buf);
        fprintf(stderr, "\nClient's request:\n%s %s\n", method, path);

        char* content_type;
        string_clear(content);
        string_clear(cookie);

        int cx, cy;

        // Generate response depending on request
        if (strcmp(method, "GET") == 0 && strcmp(path, "/data/template.css") == 0) {
            // Request for CSS file
            content_type = "text/css";
            string_append_with_size(content, p_tem->css_template, p_tem->css_template_size);
        } else if (strcmp(method, "GET") == 0 && strcmp(path, "/data/robot.png") == 0) {
            // Request for robot PNG file
            content_type = "image/png";
            string_append_with_size(content, p_tem->robot_png, p_tem->robot_png_size);
        } else if (strcmp(method, "GET") == 0 && strcmp(path, "/.well-known/appspecific/com.chrome.devtools.json") == 0) {
            // Google Chrome is to curious…
            content_type = "text/html";
            // Do nothing
        } else {
            // Other requests
            // Update coordinates
            int x_max = 4;
            int y_max = 4;
            if (strcmp(method, "POST") == 0 && strcmp(path, "/down") == 0) {
                if (x_coord == x_max) {
                    x_coord = 0;
                } else {
                    x_coord++;
                }
            } else if (strcmp(method, "POST") == 0 && strcmp(path, "/up") == 0) {
                if (x_coord == 0) {
                    x_coord = x_max;
                } else {
                    x_coord--;
                }
            } else if (strcmp(method, "POST") == 0 && strcmp(path, "/right") == 0) {
                if (y_coord == y_max) {
                    y_coord = 0;
                } else {
                    y_coord++;
                }
            } else if (strcmp(method, "POST") == 0 && strcmp(path, "/reset") == 0) {
                x_coord = 0;
                y_coord = 0;
            } else if (strcmp(method, "POST") == 0 && strcmp(path, "/left") == 0) {
                if (y_coord == 0) {
                    y_coord = y_max;
                } else {
                    y_coord--;
                }
            } else if (strcmp(method, "GET") == 0 && strcmp(path, "/") == 0) {
                // Request for head page
                // Do nothing
                fprintf(stderr, "Request main page!\n");
            } else {
                if (strcmp(method, "GET") == 0 && sscanf(path, "/coords/%d/%d", &cx, &cy) == 2) {
                    x_coord = cx;
                    y_coord = cy;
                } else {
                    fprintf(stderr, "Invalid request!\n");
                    continue;
                }
            }

            int d = string_snprintf(cookie, "Set-Cookie: x=%d\r\nSet-Cookie: y=%d\r\n", x_coord, y_coord);
            if (d < 0) {
                break;
            }

            // Generate robot grid (HTML table)
            string_clear(robot_grid);
            generate_html_table(robot_grid, x_max, y_max, x_coord, y_coord);

            // Reload HTML template at each request to see the changes
            // free(p_tem->html_template);
            // p_tem->html_template = open_and_read("./data/template.html", NULL);

            // Create HTML response
            content_type = "text/html";
            d = string_snprintf(content, p_tem->html_template, p_tem->favicon_data, x_coord, y_coord, robot_grid->start);
            if (d < 0) {
                break;
            }
        }

        // Create header for response
        int h = string_snprintf(header, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\nContent-Length: %zu\r\n%s\r\n", content_type, string_len(content), cookie->start);
        if (h < 0) {
            break;
        }

        // Concatenate header and data of response
        struct string _str;
        struct string* str = &_str;
        string_init(str);
        string_append_with_size(str, header->start, string_len(header));
        string_append_with_size(str, content->start, string_len(content));

        // Send response to client (write to client file descriptor)
        ssize_t ret = write(clientfd, str->start, string_len(str));
        if (ret < 0) {
            perror("write() failed");
            break;
        }
    }
    string_deinit(cookie);
    string_deinit(content);
    string_deinit(header);
    string_deinit(robot_grid);
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

    // Read templates
    struct templates tem;
    templates_init(&tem);

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
        handle_client(clientfd, client_addr, &tem);
        
        // Close client file descriptor
        int r = close(clientfd);
        if (r < 0) {
            perror("close() failed");
            return 1;
        }
    }
    
    templates_deinit(&tem);
}