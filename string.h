#include <stddef.h>

struct string {
    char* start;
    size_t size; // number of characters including final 0
};

void string_init(struct string* s);
void string_deinit(struct string* s);
void string_print(struct string* s);
void string_append(struct string* s, char* following, size_t len_following);