#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char charset[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64(const char* data, size_t len_data, char* res, size_t res_len) {
    int unpadded_len = (len_data*8 + 5) / 6; // ceil(data_nb_bits/6)
    int padding_len = (4 - unpadded_len) % 4;
    int size_needed = unpadded_len + padding_len;
    if (res_len < size_needed) {
        printf("Insufficient size\n");
        return size_needed;
    }

    int j = 0;
    int i = 0;
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

    for (int k = 0; k < padding_len; k++) {
        res[i++] = '=';
    }
    res[i] = 0;
    if (i != size_needed) {
        printf("i: %d / size_needed: %d\n", i, size_needed);
        puts("wat");
        exit(1);
    }
    printf("%s\n", res);
    return i;
}

int base64_str(const char* s, char* res, size_t res_len) {
    base64(s, strlen(s), res, res_len);
}

int main(void) {
    char res[37];
    int n = base64_str("Many hands make light work.", res, sizeof res);
    if (n >= sizeof res) {
        printf("%d: Size of res buffer is not enough: %d given and needs %d!\n", __LINE__, sizeof res, n);
        return 1;
    }
    n = base64_str("Man", res, sizeof res);
    if (n >= sizeof res) {
        printf("%d: Size of res buffer is not enough: %d given and needs %d!\n", __LINE__, sizeof res, n);
        return 1;
    }
    n = base64_str("Ma", res, sizeof res);
    if (n >= sizeof res) {
        printf("%d: Size of res buffer is not enough: %d given and needs %d!\n", __LINE__, sizeof res, n);
        return 1;
    }
    n = base64_str("M", res, sizeof res);
    if (n >= sizeof res) {
        printf("%d: Size of res buffer is not enough: %d given and needs %d!\n", __LINE__, sizeof res, n);
        return 1;
    }
}


// with open('./data/favicon-16x16.png', 'rb') as f:
//     data = f.read()
// base64(data)