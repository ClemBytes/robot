#include "lib_server.h"

#include <stdio.h>
#include <stdlib.h>


void templates_init(struct templates* p_tem) {
    p_tem->favicon_data = base64_from_path("./data/favicon-16x16.png", NULL);
    p_tem->html_template = open_and_read("./data/template.html", NULL);
    p_tem->css_template = open_and_read("./data/template.css", &p_tem->css_template_size);
    p_tem->robot_png = open_and_read("./data/robot.png", &p_tem->robot_png_size);
}

void templates_deinit(struct templates* p_tem) {
    free(p_tem->favicon_data);
    free(p_tem->html_template);
    free(p_tem->css_template);
    free(p_tem->robot_png);
}

void parse_client_request(const char* client_request, size_t data_len, char* method, char* path, char* version, int* p_cookie_x, int* p_cookie_y, int* p_cookie_found) {
    // Get first line of request
    size_t i;
    struct string _first_line;
    struct string* first_line = &_first_line;
    string_init(first_line);
    for (i = 0; i < data_len; i++) {
        if (client_request[i] == '\r'){
            string_append_with_size(first_line, "\r", 1);
            string_append_with_size(first_line, "\n", 1);
            break;
        } else if (client_request[i] == '\n') {
            string_append_with_size(first_line, "\r", 1);
            string_append_with_size(first_line, "\n", 1);
            break;
        } else {
            string_append_with_size(first_line, &client_request[i], 1);
        }
    }
    if (i >= first_line->used_size - 1) {
        fprintf(stderr, "Size of first line is not enough: %zu given and needs %zu!\n", first_line->used_size, i);
        return;
    }
    string_append_with_size(first_line, "\0", 1);

    // Parse first line
    int nb_match = sscanf(first_line->start, "%15s %1023s %15s", method, path, version);
    if (nb_match != 3) {
        fprintf(stderr, "Uncomplete request first line\n");
        return;
    }
    // printf("-------------------------------------\n");
    // printf("Method: %s\n", method);
    // printf("Path: %s\n", path);
    // printf("Version: %s\n", version);
    // printf("-------------------------------------\n");
    // fflush(stdout);

    // Parse following lines to get cookies
    (*p_cookie_found) = 0;
    // Initialize new line
    struct string _new_line;
    struct string* new_line = &_new_line;
    string_init(new_line);
    while (i < data_len) {
        // Detect end of line
        if ((client_request[i] == '\r') || (client_request[i] == '\n')){
            string_append_with_size(new_line, "\r", 1);
            string_append_with_size(new_line, "\n", 1);
            string_append_with_size(new_line, "\0", 1);
            
            // End of line detected: check if cookies
            int nb_match_cookies = sscanf(new_line->start, "Cookie: x=%d; y=%d\r\n", p_cookie_x, p_cookie_y);
            if (nb_match_cookies == 2) {
                // Cookies detected!
                (*p_cookie_found) = 1;
                fprintf(stderr, "%s:%d - Cookies found: x: %d | y: %d\r\n", __FILE__, __LINE__, *p_cookie_x, *p_cookie_y);
                break;
            } else {
                nb_match_cookies = sscanf(new_line->start, "Cookie: y=%d; x=%d\r\n", p_cookie_y, p_cookie_x);
                if (nb_match_cookies == 2) {
                    // Cookies detected!
                    (*p_cookie_found) = 1;
                    fprintf(stderr, "%s:%d - Cookies found: x: %d | y: %d\r\n", __FILE__, __LINE__, *p_cookie_x, *p_cookie_y);
                    break;
                } else {
                    // Re-init for new line
                    string_init(new_line);
                }
            }

        } else {
            string_append_with_size(new_line, &client_request[i], 1);
        }
        i++;
    }
    if (!(*p_cookie_found)) {
        printf("%s:%d - Cookies NOT found!\n", __FILE__, __LINE__);
    }
}

void generate_html_table(struct string* robot_grid, const int x_max, const int y_max, const int x_coord, const int y_coord) {
    struct string _td;
    struct string* td = &_td;
    string_init(td);
    for (int x = 0; x < x_max + 1; x++) {
        string_append(robot_grid, "<tr>");
        for (int y = 0; y < y_max + 1; y++) {
            if ((x == x_coord) && (y == y_coord)) {
                string_snprintf(td, "<td data-x='%d' data-y='%d'><img src='/data/robot.png' alt='Robot' class='image-responsive'></td>", x, y);
                string_append(robot_grid, td->start);
                string_clear(td);
            } else {
                string_snprintf(td, "<td data-x='%d' data-y='%d'></td>", x, y);
                string_append(robot_grid, td->start);
                string_clear(td);
            }
        }
        string_append(robot_grid, "</tr>");
    }
    string_deinit(td);
}