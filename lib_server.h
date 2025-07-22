#include <sys/types.h>

#include "string.h"
#include "base64.h"

struct templates {
    char* favicon_data;
    char* html_template;
    size_t css_template_size;
    char* css_template;
    size_t robot_png_size;
    char* robot_png;
};

void templates_init(struct templates* p_tem);
void templates_deinit(struct templates* p_tem);
void parse_client_request(const char* client_request, size_t data_len, char* method, char* path, char* version, int* p_cookie_x, int* p_cookie_y, int* p_cookie_found);
void generate_html_table(struct string* robot_grid, const int x_max, const int y_max, const int x_coord, const int y_coord);