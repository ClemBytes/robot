#include "base64.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

const char charset[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t base64(const unsigned char* data, size_t len_data, char* res, size_t res_len) {
    size_t unpadded_len = (len_data*8 + 5) / 6; // ceil(data_nb_bits/6)
    size_t padding_len = (4 - unpadded_len) % 4;
    size_t size_needed = unpadded_len + padding_len;
    if (res_len < size_needed) {
        return size_needed;
    }

    size_t j = 0;
    size_t i = 0;
    while (j + 2 < len_data) {
        res[i++] = charset[data[j + 0] >> 2];
        res[i++] = charset[((data[j + 0] & 0b00000011) << 4) | (data[j + 1] >> 4)];
        res[i++] = charset[((data[j + 1] & 0b00001111) << 2) | (data[j + 2] >> 6)];
        res[i++] = charset[data[j + 2] & 0b00111111];
        j += 3;
    }

    if (len_data - j == 1) {
        res[i++] = charset[data[j + 0] >> 2];
        res[i++] = charset[((data[j + 0] & 0b00000011) << 4) | (0 >> 4)];
    } else if (len_data - j == 2) {
        res[i++] = charset[data[j + 0] >> 2];
        res[i++] = charset[((data[j + 0] & 0b00000011) << 4) | (data[j + 1] >> 4)];
        res[i++] = charset[((data[j + 1] & 0b00001111) << 2) | (0 >> 6)];
    }

    while (padding_len--) {
        res[i++] = '=';
    }
    res[i] = 0;
    if (i != size_needed) {
        printf("i: %zu / size_needed: %zu\n", i, size_needed);
        puts("wat");
        exit(1);
    }
    return i;
}

size_t base64_str(const char* s, char* res, size_t res_len) {
    base64(s, strlen(s), res, res_len);
}

char* open_and_read(const char* path, size_t* file_size_out) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open() failed in open_and_read");
        return NULL;
    }
    // Get file size
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size < 0) {
        perror("First lseek() failed in open_and_read");
        close(fd);
        return NULL;
    }
    // Come back to beginning of file
    off_t r = lseek(fd, 0, SEEK_SET);
    if (r < 0) {
        perror("Second lseek() failed in open_and_read");
        close(fd);
        return NULL;
    }
    // Read file
    char* data = malloc(file_size + 1);
    if (data == NULL) {
        perror("malloc() open_and_read failed");
        close(fd);
        return NULL;
    }
    int bytes_read = read(fd, data, file_size);
    if (bytes_read < 0) {
        perror("read() failed");
        close(fd);
        return NULL;
    }
    if (bytes_read != file_size) {
        printf("Bytes read (%d) doesn't match file size (%zi)\n", bytes_read, file_size);
        close(fd);
        return NULL;
    }
    data[file_size] = 0;
    // Close file
    int e = close(fd);
    if (e < 0) {
        perror("close() failed");
        return NULL;
    }
    if (file_size_out != NULL) {
        *file_size_out = file_size;
    }
    return data;
}

char* base64_alloc(const char* data, size_t size, size_t* res_size_out) {
    size_t res_size = base64(data, size, NULL, 0) + 1;
    char* res_png = malloc(res_size);
    if (res_png == NULL) {
        perror("malloc() base64_alloc failed");
        return NULL;
    }
    base64(data, size, res_png, res_size);
    if (res_size_out != NULL) {
        *res_size_out = res_size;
    }
    return res_png;
}

char* base64_from_path(const char* path, size_t* res_size_out) {
    // Open favicon PNG file
    size_t file_size;
    char* data = open_and_read(path, &file_size);
    if (data == NULL) {
        printf("open_and_read() failed\n");
        return NULL;
    }

    // Convert with base64
    char* res_png = base64_alloc(data, file_size, res_size_out);
    if (res_png == NULL) {
        printf("base64_alloc() failed\n");
        free(data);
        return NULL;
    }
    free(data);
    return res_png;
}