#include <stddef.h>
#include <stdarg.h>

struct string {
    char* start;
    size_t size; // number of characters disponible (including already written and final 0)
    size_t used_size; // number of characters including final 0
};

void string_init(struct string* s);
void string_deinit(struct string* s);
size_t string_len(struct string* s);
void string_print(struct string* s);
void string_append_with_size(struct string* s, const char* following, size_t len_following);
void string_append(struct string* s, const char* following);
int string_snprintf(struct string* s, const char* format, ...);

#define string_append_literal(x, y) string_append_with_size(x, y, (sizeof y) - 1)