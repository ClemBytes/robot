#include <sys/types.h>

#include "string.h"
#include "base64.h"

/**
 * @struct templates
 * @brief A structure for storing data used by HTTP responses.
 * 
 * - favicon_data: pointer to the base64 encoding of the favicon.
 * - html_template: pointer to the format string of the HTML file.
 * - css_template_size: size of the css template file.
 * - css_template: pointer to the css template text.
 * - robot_png_size: size of the robot PNG file.
 * - robot_png: pointer to the robot PNG file binary.
 */
struct templates {
    char* favicon_data;
    char* html_template;
    size_t css_template_size;
    char* css_template;
    size_t robot_png_size;
    char* robot_png;
};

/**
 * @brief Initializes a template structure by opening and reading all files.
 * 
 * Uses the open_and_read() function from base64.h.
 * 
 * @param p_tem Pointer to the template structure to initialize.
 */
void templates_init(struct templates* p_tem);

/**
 * @brief Frees (de-initializes) memory used by a template structure.
 * 
 * @param p_tem Pointer to the template structure to de-initialize.
 */
void templates_deinit(struct templates* p_tem);

/**
 * @brief Parses a client request and gets all needed information:
 * 
 * From the first line of the request:
 * - method (e.g. GET, POST…)
 * - path (e.g. /data/template.css)
 * - HTTP version (e.g. HTTP/1)
 * 
 * From the cookies:
 * - Are there cookies or not ?
 * - If yes, x and y coordinates of robot.
 * 
 * @param client_request Pointer to the client's request string.
 * @param data_len Size of client's request.
 * @param method Pointer for the method (initialze with `char method[16];`).
 * @param path Pointer for the path. (initialze with `char path[1024];`).
 * @param version Pointer for the version (initialze with `char version[16];`).
 * @param p_cookie_x Pointer for the x coordinate in the cookies.
 * @param p_cookie_y Pointer for the y coordinate in the cookies.
 * @param p_cookie_found Pointer for a boolean to know if there is a cookie:
 *                       1 if cookie was found, otherwise 0.
 */
void parse_client_request(const char* client_request, size_t data_len, char* method, char* path, char* version, int* p_cookie_x, int* p_cookie_y, int* p_cookie_found);

/**
 * @brief Generates the HTML table with the robot in the good case from
 *        given coordinates.
 * 
 * @param robot_grid Pointer for the result.
 * @param x_max Value max for x (so number of lines in the grid).
 * @param y_max Value max for y (so number of columns in the grid).
 * @param x_coord Robot's x coordinate.
 * @param y_coord Robot's y coordinate.
 */
void generate_html_table(struct string* robot_grid, const int x_max, const int y_max, const int x_coord, const int y_coord);