#include "string.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void string_init(struct string* s) {
    s->start = malloc(1); // (*s).start = NULL;
    s->start[0] = 0;
    s->size = 1;
}

void string_deinit(struct string* s) {
    free(s->start);
}

void string_print(struct string* s) {
    printf("String: %s - start: %p - size: %zu\n", s->start, s->start, s->size);
}

void string_append(struct string* s, char* following, size_t len_following) {
    s->start = realloc(s->start, s->size + len_following);
    // buffer at *start contains size - 1 characters, followed by 1 zero
    memcpy(s->start + s->size - 1, following, len_following);
    // buffer at *start contains size - 1 + len_following characters
    s->size += len_following - 1;
    // buffer at *start contains size characters
    s->start[s->size] = 0;
    // buffer at *start contains size characters, followed by 1 zero
    s->size += 1;
    // buffer at *start contains size - 1 characters, followed by 1 zero
}