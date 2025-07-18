#include <stddef.h>

struct string {
    char* start;
    size_t size; // number of characters disponible (including already written and final 0)
    size_t used_size; // number of characters including final 0
};

void string_init(struct string* s);
void string_deinit(struct string* s);
void string_print(struct string* s);
void string_append_with_size(struct string* s, char* following, size_t len_following);
void string_append(struct string* s, char* following);

#define string_append_literal(x, y) string_append_with_size(x, y, (sizeof y) - 1)