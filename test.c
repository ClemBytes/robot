#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "base64.h"

void test_base64(const char* input, const char* expected) {
    char res[100];
    size_t n = base64_str(input, res, sizeof res);
    if (n >= sizeof res) {
        printf("Size of res buffer is not enough: %d given and needs %d!\n", sizeof res, n);
        exit(1);
    }
    if (strcmp(res, expected) == 0) {
        printf("%s: %s\n", input, res);
    } else {
        printf("UNEXPECTED RESULT!\nInitial: %s\nResult: %s\nExpected: %s\n", input, res, expected);
        exit(1);
    }
}

int main(void) {
    test_base64("Many hands make light work.", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu");
    test_base64("Man", "TWFu");
    test_base64("Ma", "TWE=");
    test_base64("M", "TQ==");

    char* res_png = base64_from_path("./data/favicon-16x16.png", NULL);
    if (res_png == NULL) {
        printf("base64_from_path() failed!\n");
        return 1;
    }
    printf("PNG image:\n%s\n", res_png);
    free(res_png);
    return 0;
}