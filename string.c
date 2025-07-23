#include "string.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void string_init(struct string* s) {
    s->start = malloc(1); // (*s).start = NULL;
    s->start[0] = 0;
    s->size = 1;
    s->used_size = 1;
}

void string_deinit(struct string* s) {
    free(s->start);
}

void string_print(struct string* s) {
    printf("String: %s - start: %p - used size: %zu - size: %zu\n", s->start, s->start, s->used_size, s->size);
}

void string_append_with_size(struct string* s, const char* following, size_t len_following) {
    while (s->used_size + len_following > s->size) {
        s->start = realloc(s->start, 2*s->size);
        s->size = 2*s->size;
        // printf("REALLOC!! New size: %zu\n", s->size);
    }
    // buffer at *start contains size - 1 characters, followed by 1 zero
    memcpy(s->start + s->used_size - 1, following, len_following);
    // buffer at *start contains size - 1 + len_following characters
    s->used_size += len_following - 1;
    // buffer at *start contains size characters
    s->start[s->used_size] = 0;
    // buffer at *start contains size characters, followed by 1 zero
    s->used_size += 1;
    // buffer at *start contains size - 1 characters, followed by 1 zero
}

void string_append(struct string* s, const char* following) {
    size_t len_following = strlen(following);
    string_append_with_size(s, following, len_following);
}

int string_snprintf(struct string* s, const char* format, ...) {
    // Initialize variable argument list
    va_list args;
    va_start(args, format);
    // Copy for second use
    va_list args_copy;
    va_copy(args_copy, args);
    
    // Get length of result
    int length = vsnprintf(NULL, 0, format, args);
    if (length < 0) {
        perror("vsnprintf() for string_snprintf() failed");
        string_deinit(s);
        va_end(args);
        va_end(args_copy);
        return length;
    }
    va_end(args);

    // Allocate memory
    s->start = malloc(length + 1);
    s->size = length + 1;
    s->used_size = length + 1;

    // Then do the format string
    int r = vsnprintf(s->start, length + 1, format, args_copy);
    if (r < 0) {
        perror("vsnprintf() for string_snprintf() failed");
        va_end(args_copy);
        string_deinit(s);
        return r;
    } else if (r >= length + 1) {
        printf("%s:%d - Size of string is not enough: %d given and needs %d!\n", __FILE__, __LINE__, length + 1, r);
        string_deinit(s);
        va_end(args_copy);
        return -1;
    }

    // Free memory used by va_list
    va_end(args_copy);

    // Return result of vsnprintf
    return r;
}