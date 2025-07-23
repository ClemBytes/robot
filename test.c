#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "base64.h"
#include "lib_server.h"

void test_base64_case(const char* input, const char* expected) {
    char res[100];
    size_t n = base64_str(input, res, sizeof res);
    if (n >= sizeof res) {
        fprintf(stderr, "Size of res buffer is not enough: %zu given and needs %zu!\n", sizeof res, n);
        exit(1);
    }
    if (strcmp(res, expected) == 0) {
        fprintf(stderr, "%s: %s\n", input, res);
    } else {
        fprintf(stderr, "UNEXPECTED RESULT!\nInitial: %s\nResult: %s\nExpected: %s\n", input, res, expected);
        exit(1);
    }
}

void test_base64(void) {
    printf("-----------------------------------\n");
    printf("Tests BASE64:\n");
    printf("-------------\n");
    test_base64_case("Many hands make light work.", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu");
    test_base64_case("Man", "TWFu");
    test_base64_case("Ma", "TWE=");
    test_base64_case("M", "TQ==");

    char* res_png = base64_from_path("./data/favicon-16x16.png", NULL);
    if (res_png == NULL) {
        fprintf(stderr, "base64_from_path() failed!\n");
        exit(1);
    }
    printf("PNG image:\n%s\n", res_png);
    free(res_png);
    printf("\n--- BASE64 tests OK! ---\n");
    printf("-----------------------------------\n");
}

void test_string(void) {
    printf("-----------------------------------\n");
    printf("Tests STRING:\n");
    printf("-------------\n");
    // Init
    struct string _s;
    struct string* s = &_s;
    string_init(s);
    string_print(s);

    // Append text
    string_append_literal(s, "Hello world"); // 11 chars + 0-term = 12 chars
    string_print(s);
    if (s->start[string_len(s)] != 0) {
        fprintf(stderr, "%s:%d - string is not zero-terminated!\n", __FILE__, __LINE__);
        exit(1);
    }
    if (strlen(s->start) != strlen("Hello world")) {
        fprintf(stderr, "%s:%d - string does not have right size (WITHOUT 0-term)! Should be %zu and is %zu\n", __FILE__, __LINE__, strlen("Hello world"), strlen(s->start));
        exit(1);
    }
    if (s->used_size != sizeof "Hello world") {
        fprintf(stderr, "%s:%d - string does not have right size (WITH 0-term)! Should be %zu and is %zu\n", __FILE__, __LINE__, sizeof "Hello world", s->used_size);
        exit(1);
    }


    // Append more text
    string_append(s, " + Coucou"); // 11 chars + 9 chars + 0-term = 21 chars
    string_print(s);
    if (s->start[string_len(s)] != 0) {
        fprintf(stderr, "%s:%d - string is not zero-terminated!\n", __FILE__, __LINE__);
        exit(1);
    }
    if (strlen(s->start) != strlen("Hello world + Coucou")) {
        fprintf(stderr, "%s:%d - string does not have right size (WITHOUT 0-term)! Should be %zu and is %zu\n", __FILE__, __LINE__, strlen("Hello world + Coucou"), strlen(s->start));
        exit(1);
    }
    if (s->used_size != sizeof "Hello world + Coucou") {
        fprintf(stderr, "%s:%d - string does not have right size (WITH 0-term)! Should be %zu and is %zu\n", __FILE__, __LINE__, sizeof "Hello world + Coucou", s->used_size);
        exit(1);
    }

    // Free memory
    string_deinit(s);
    printf("\n--- STRING tests OK! ---\n");
    printf("-----------------------------------\n");
}

void test_string_snprintf(void) {
    printf("-----------------------------------\n");
    printf("Tests STRING SNPRINTF:\n");
    printf("----------------------\n");
    int r;

    // 1. One argument
    struct string _s1;
    struct string* s1 = &_s1;
    string_init(s1);
    r =  string_snprintf(s1, "Value: %d", 42);
    if (r < 0) {
        fprintf(stderr, "%s:%d - string_snprintf() test 1 (one argument) failed\n", __FILE__, __LINE__);
        exit(1);
    }
    if (strcmp(s1->start, "Value: 42") != 0) {
        fprintf(stderr, "%s:%d - Test 1 (one argument) failed!\nExpected: <Value: 42>\nResult: <%s>\n", __FILE__, __LINE__, s1->start);
        exit(1);
    }
    string_deinit(s1);
    printf("> Test 1 OK: one argument\n");

    // 2. No arguments
    struct string _s2;
    struct string* s2 = &_s2;
    string_init(s2);
    r =  string_snprintf(s2, "Hello");
    if (r < 0) {
        fprintf(stderr, "%s:%d - string_snprintf() test 2 (no arguments) failed\n", __FILE__, __LINE__);
        exit(1);
    }
    if (strcmp(s2->start, "Hello") != 0) {
        fprintf(stderr, "%s:%d - Test 2 (no arguments) failed!\nExpected: <Hello>\nResult: <%s>\n", __FILE__, __LINE__, s2->start);
        exit(1);
    }
    string_deinit(s2);
    printf("> Test 2 OK: no arguments\n");

    // 3. Insert char*
    struct string _s3;
    struct string* s3 = &_s3;
    string_init(s3);
    r =  string_snprintf(s3, "Hi %s", "world");
    if (r < 0) {
        fprintf(stderr, "%s:%d - string_snprintf() test 3 (insert char*) failed\n", __FILE__, __LINE__);
        exit(1);
    }
    if (strcmp(s3->start, "Hi world") != 0) {
        fprintf(stderr, "%s:%d - Test 3 (insert char*) failed!\nExpected: <Hi world>\nResult: <%s>\n", __FILE__, __LINE__, s3->start);
        exit(1);
    }
    string_deinit(s3);
    printf("> Test 3 OK: insert char*\n");

    // 4. Insert several types
    struct string _s4;
    struct string* s4 = &_s4;
    string_init(s4);
    r =  string_snprintf(s4, "%s %d %.2f", "Test", 42, 3.14);
    if (r < 0) {
        fprintf(stderr, "%s:%d - string_snprintf() test 4 (insert several types) failed\n", __FILE__, __LINE__);
        exit(1);
    }
    if (strcmp(s4->start, "Test 42 3.14") != 0) {
        fprintf(stderr, "%s:%d - Test 4 (insert several types) failed!\nExpected: <Test 42 3.14>\nResult: <%s>\n", __FILE__, __LINE__, s4->start);
        exit(1);
    }
    string_deinit(s4);
    printf("> Test 4 OK: insert several types\n");

    // 5. Empty string
    struct string _s5;
    struct string* s5 = &_s5;
    string_init(s5);
    r =  string_snprintf(s5, "");
    if (r < 0) {
        fprintf(stderr, "%s:%d - string_snprintf() test 5 (empty string) failed\n", __FILE__, __LINE__);
        exit(1);
    }
    if (strcmp(s5->start, "") != 0) {
        fprintf(stderr, "%s:%d - Test 5 (empty string) failed!\nExpected: <>\nResult: <%s>\n", __FILE__, __LINE__, s5->start);
        exit(1);
    }
    string_deinit(s5);
    printf("> Test 5 OK: empty string\n");

    // 6. NULL as argument
    struct string _s6;
    struct string* s6 = &_s6;
    string_init(s6);
    r =  string_snprintf(s6, "Hey %s", NULL);
    if (r < 0) {
        fprintf(stderr, "%s:%d - string_snprintf() test 6 (NULL as argument) failed\n", __FILE__, __LINE__);
        exit(1);
    }
    if (strcmp(s6->start, "Hey (null)") != 0) {
        fprintf(stderr, "%s:%d - Test 6 (NULL as argument) failed!\nExpected: <Hey (null)>\nResult: <%s>\n", __FILE__, __LINE__, s6->start);
        exit(1);
    }
    string_deinit(s6);
    printf("> Test 6 OK: NULL as argument\n");

    // 7. Very long string
    struct string _s7;
    struct string* s7 = &_s7;
    string_init(s7);
    r =  string_snprintf(s7, "A%01000d", 1);
    if (r < 0) {
        fprintf(stderr, "%s:%d - string_snprintf() test 7 (very long string) failed\n", __FILE__, __LINE__);
        exit(1);
    }
    if (s7->used_size <= 1000) {
        fprintf(stderr, "%s:%d - Test 7 (very long string) failed! Too short!\nLength expected > 1000\nResult's length: <%zu>\n", __FILE__, __LINE__, s7->used_size);
        exit(1);
    }
    string_deinit(s7);
    printf("> Test 7 OK: very long string\n");

    // 8. String in string
    struct string _s8;
    struct string* s8 = &_s8;
    string_init(s8);
    struct string _s8_bis;
    struct string* s8_bis = &_s8_bis;
    string_init(s8_bis);
    string_append_literal(s8_bis, "world");
    r =  string_snprintf(s8, "Hello <%s> !", s8_bis->start);
    if (r < 0) {
        fprintf(stderr, "%s:%d - string_snprintf() test 8 (string in string) failed\n", __FILE__, __LINE__);
        exit(1);
    }
    if (strcmp(s8->start, "Hello <world> !") != 0) {
        fprintf(stderr, "%s:%d - Test 8 (string in string) failed!\nExpected: <Hello <world> !>\nResult: <%s>\n", __FILE__, __LINE__, s8->start);
        exit(1);
    }
    string_deinit(s8);
    printf("> Test 8 OK: string in string\n");

    printf("\n--- STRING SNPRINTF tests OK! ---\n");
    printf("-----------------------------------\n");
}

void test_parse_client_request(void) {
    printf("-----------------------------------\n");
    printf("Tests PARSE REQUEST:\n");
    printf("--------------------\n");

    char method[16], path[1024], version[16];
    int cookie_x, cookie_y, cookie_found;
    
    // Request 1
    size_t data_len1;
    char* request1 = open_and_read("./data/test_parse/request1", &data_len1);
    parse_client_request(request1, data_len1, method, path, version, &cookie_x, &cookie_y, &cookie_found);
    if (strcmp(method, "GET") != 0) {
        fprintf(stderr, "--- REQUEST 1 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: GET\n", __FILE__, __LINE__, method);
        exit(1);
    }
    if (strcmp(path, "/") != 0) {
        fprintf(stderr, "--- REQUEST 1 ---\n");
        fprintf(stderr, "%s:%d - Wrong PATH: parsed: %s | expected: /\n", __FILE__, __LINE__, path);
        exit(1);
    }
    if (strcmp(version, "HTTP/1") != 0) {
        fprintf(stderr, "--- REQUEST 1 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: HTTP/1\n", __FILE__, __LINE__, version);
        exit(1);
    }
    if (cookie_found != 0) {
        fprintf(stderr, "--- REQUEST 1 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_FOUND: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_found);
        exit(1);
    }
    free(request1);
    printf("> Request 1 OK\n");
    
    // Request 2
    size_t data_len2;
    char* request2 = open_and_read("./data/test_parse/request2", &data_len2);
    parse_client_request(request2, data_len2, method, path, version, &cookie_x, &cookie_y, &cookie_found);
    if (strcmp(method, "GET") != 0) {
        fprintf(stderr, "--- REQUEST 2 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: GET\n", __FILE__, __LINE__, method);
        exit(1);
    }
    if (strcmp(path, "/data/template.css") != 0) {
        fprintf(stderr, "--- REQUEST 2 ---\n");
        fprintf(stderr, "%s:%d - Wrong PATH: parsed: %s | expected: /data/template.css\n", __FILE__, __LINE__, path);
        exit(1);
    }
    if (strcmp(version, "HTTP/1") != 0) {
        fprintf(stderr, "--- REQUEST 2 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: HTTP/1\n", __FILE__, __LINE__, version);
        exit(1);
    }
    if (cookie_found != 1) {
        fprintf(stderr, "--- REQUEST 2 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_FOUND: parsed: %d | expected: 1\n", __FILE__, __LINE__, cookie_found);
        exit(1);
    }
    if (cookie_x != 0) {
        fprintf(stderr, "--- REQUEST 2 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_X: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_x);
        exit(1);
    }
    if (cookie_y != 0) {
        fprintf(stderr, "--- REQUEST 2 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_Y: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_y);
        exit(1);
    }
    free(request2);
    printf("> Request 2 OK\n");
    
    // Request 3
    size_t data_len3;
    char* request3 = open_and_read("./data/test_parse/request3", &data_len3);
    parse_client_request(request3, data_len3, method, path, version, &cookie_x, &cookie_y, &cookie_found);
    if (strcmp(method, "GET") != 0) {
        fprintf(stderr, "--- REQUEST 3 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: GET\n", __FILE__, __LINE__, method);
        exit(1);
    }
    if (strcmp(path, "/data/robot.png") != 0) {
        fprintf(stderr, "--- REQUEST 3 ---\n");
        fprintf(stderr, "%s:%d - Wrong PATH: parsed: %s | expected: /data/robot.png\n", __FILE__, __LINE__, path);
        exit(1);
    }
    if (strcmp(version, "HTTP/1") != 0) {
        fprintf(stderr, "--- REQUEST 3 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: HTTP/1\n", __FILE__, __LINE__, version);
        exit(1);
    }
    if (cookie_found != 1) {
        fprintf(stderr, "--- REQUEST 3 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_FOUND: parsed: %d | expected: 1\n", __FILE__, __LINE__, cookie_found);
        exit(1);
    }
    if (cookie_x != 0) {
        fprintf(stderr, "--- REQUEST 3 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_X: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_x);
        exit(1);
    }
    if (cookie_y != 0) {
        fprintf(stderr, "--- REQUEST 3 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_Y: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_y);
        exit(1);
    }
    free(request3);
    printf("> Request 3 OK\n");
    
    // Request 4
    size_t data_len4;
    char* request4 = open_and_read("./data/test_parse/request4", &data_len4);
    parse_client_request(request4, data_len4, method, path, version, &cookie_x, &cookie_y, &cookie_found);
    if (strcmp(method, "POST") != 0) {
        fprintf(stderr, "--- REQUEST 4 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: POST\n", __FILE__, __LINE__, method);
        exit(1);
    }
    if (strcmp(path, "/reset") != 0) {
        fprintf(stderr, "--- REQUEST 4 ---\n");
        fprintf(stderr, "%s:%d - Wrong PATH: parsed: %s | expected: /reset\n", __FILE__, __LINE__, path);
        exit(1);
    }
    if (strcmp(version, "HTTP/1") != 0) {
        fprintf(stderr, "--- REQUEST 4 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: HTTP/1\n", __FILE__, __LINE__, version);
        exit(1);
    }
    if (cookie_found != 1) {
        fprintf(stderr, "--- REQUEST 4 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_FOUND: parsed: %d | expected: 1\n", __FILE__, __LINE__, cookie_found);
        exit(1);
    }
    if (cookie_x != 0) {
        fprintf(stderr, "--- REQUEST 4 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_X: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_x);
        exit(1);
    }
    if (cookie_y != 0) {
        fprintf(stderr, "--- REQUEST 4 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_Y: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_y);
        exit(1);
    }
    free(request4);
    printf("> Request 4 OK\n");
    
    // Request 5
    size_t data_len5;
    char* request5 = open_and_read("./data/test_parse/request5", &data_len5);
    parse_client_request(request5, data_len5, method, path, version, &cookie_x, &cookie_y, &cookie_found);
    if (strcmp(method, "POST") != 0) {
        fprintf(stderr, "--- REQUEST 5 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: POST\n", __FILE__, __LINE__, method);
        exit(1);
    }
    if (strcmp(path, "/left") != 0) {
        fprintf(stderr, "--- REQUEST 5 ---\n");
        fprintf(stderr, "%s:%d - Wrong PATH: parsed: %s | expected: /left\n", __FILE__, __LINE__, path);
        exit(1);
    }
    if (strcmp(version, "HTTP/1") != 0) {
        fprintf(stderr, "--- REQUEST 5 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: HTTP/1\n", __FILE__, __LINE__, version);
        exit(1);
    }
    if (cookie_found != 1) {
        fprintf(stderr, "--- REQUEST 5 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_FOUND: parsed: %d | expected: 1\n", __FILE__, __LINE__, cookie_found);
        exit(1);
    }
    if (cookie_x != 2) {
        fprintf(stderr, "--- REQUEST 5 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_X: parsed: %d | expected: 2\n", __FILE__, __LINE__, cookie_x);
        exit(1);
    }
    if (cookie_y != 4) {
        fprintf(stderr, "--- REQUEST 5 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_Y: parsed: %d | expected: 4\n", __FILE__, __LINE__, cookie_y);
        exit(1);
    }
    free(request5);
    printf("> Request 5 OK\n");
    
    // Request 6
    size_t data_len6;
    char* request6 = open_and_read("./data/test_parse/request6", &data_len6);
    parse_client_request(request6, data_len6, method, path, version, &cookie_x, &cookie_y, &cookie_found);
    if (strcmp(method, "GET") != 0) {
        fprintf(stderr, "--- REQUEST 6 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: GET\n", __FILE__, __LINE__, method);
        exit(1);
    }
    if (strcmp(path, "/.well-known/appspecific/com.chrome.devtools.json") != 0) {
        fprintf(stderr, "--- REQUEST 6 ---\n");
        fprintf(stderr, "%s:%d - Wrong PATH: parsed: %s | expected: /.well-known/appspecific/com.chrome.devtools.json\n", __FILE__, __LINE__, path);
        exit(1);
    }
    if (strcmp(version, "HTTP/1.1") != 0) {
        fprintf(stderr, "--- REQUEST 6 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: HTTP/1.1\n", __FILE__, __LINE__, version);
        exit(1);
    }
    if (cookie_found != 1) {
        fprintf(stderr, "--- REQUEST 6 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_FOUND: parsed: %d | expected: 1\n", __FILE__, __LINE__, cookie_found);
        exit(1);
    }
    if (cookie_x != 0) {
        fprintf(stderr, "--- REQUEST 6 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_X: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_x);
        exit(1);
    }
    if (cookie_y != 0) {
        fprintf(stderr, "--- REQUEST 6 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_Y: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_y);
        exit(1);
    }
    free(request6);
    printf("> Request 6 OK\n");
    
    // Request 7
    size_t data_len7;
    char* request7 = open_and_read("./data/test_parse/request7", &data_len7);
    parse_client_request(request7, data_len7, method, path, version, &cookie_x, &cookie_y, &cookie_found);
    if (strcmp(method, "GET") != 0) {
        fprintf(stderr, "--- REQUEST 7 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: GET\n", __FILE__, __LINE__, method);
        exit(1);
    }
    if (strcmp(path, "/data/robot.png") != 0) {
        fprintf(stderr, "--- REQUEST 7 ---\n");
        fprintf(stderr, "%s:%d - Wrong PATH: parsed: %s | expected: /data/robot.png\n", __FILE__, __LINE__, path);
        exit(1);
    }
    if (strcmp(version, "HTTP/1.1") != 0) {
        fprintf(stderr, "--- REQUEST 7 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: HTTP/1.1\n", __FILE__, __LINE__, version);
        exit(1);
    }
    if (cookie_found != 1) {
        fprintf(stderr, "--- REQUEST 7 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_FOUND: parsed: %d | expected: 1\n", __FILE__, __LINE__, cookie_found);
        exit(1);
    }
    if (cookie_x != 2) {
        fprintf(stderr, "--- REQUEST 7 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_X: parsed: %d | expected: 2\n", __FILE__, __LINE__, cookie_x);
        exit(1);
    }
    if (cookie_y != 0) {
        fprintf(stderr, "--- REQUEST 7 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_Y: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_y);
        exit(1);
    }
    free(request7);
    printf("> Request 7 OK\n");
    
    // Request 8
    size_t data_len8;
    char* request8 = open_and_read("./data/test_parse/request8", &data_len8);
    parse_client_request(request8, data_len8, method, path, version, &cookie_x, &cookie_y, &cookie_found);
    if (strcmp(method, "POST") != 0) {
        fprintf(stderr, "--- REQUEST 8 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: POST\n", __FILE__, __LINE__, method);
        exit(1);
    }
    if (strcmp(path, "/right") != 0) {
        fprintf(stderr, "--- REQUEST 8 ---\n");
        fprintf(stderr, "%s:%d - Wrong PATH: parsed: %s | expected: /right\n", __FILE__, __LINE__, path);
        exit(1);
    }
    if (strcmp(version, "HTTP/1.1") != 0) {
        fprintf(stderr, "--- REQUEST 8 ---\n");
        fprintf(stderr, "%s:%d - Wrong METHOD: parsed: %s | expected: HTTP/1.1\n", __FILE__, __LINE__, version);
        exit(1);
    }
    if (cookie_found != 1) {
        fprintf(stderr, "--- REQUEST 8 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_FOUND: parsed: %d | expected: 1\n", __FILE__, __LINE__, cookie_found);
        exit(1);
    }
    if (cookie_x != 2) {
        fprintf(stderr, "--- REQUEST 8 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_X: parsed: %d | expected: 2\n", __FILE__, __LINE__, cookie_x);
        exit(1);
    }
    if (cookie_y != 0) {
        fprintf(stderr, "--- REQUEST 8 ---\n");
        fprintf(stderr, "%s:%d - Wrong COOKIE_Y: parsed: %d | expected: 0\n", __FILE__, __LINE__, cookie_y);
        exit(1);
    }
    free(request8);
    printf("> Request 8 OK\n");

    printf("\n--- PARSE REQUEST tests OK! ---\n");
    printf("-----------------------------------\n");

}

int main(void) {
    test_base64();
    test_string();
    test_string_snprintf();
    test_parse_client_request();
    return 0;
}