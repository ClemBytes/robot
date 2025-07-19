#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "base64.h"
#include "string.h"

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
    printf("---------------------------\n");
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
    printf("---------------------------\n");
}

void test_string(void) {
    printf("---------------------------\n");
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
    if (s->start[s->used_size - 1] != 0) {
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
    if (s->start[s->used_size - 1] != 0) {
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
    printf("---------------------------\n");
}

int main(void) {
    test_base64();
    test_string();
    return 0;
}