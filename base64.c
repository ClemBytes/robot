#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char charset[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64(const char* data, size_t len_data, char* res, size_t res_len) {
    int j = 0;
    int i = 0;
    while (j + 2 < len_data) {
        res[i + 0] = charset[data[j + 0] >> 2];
        res[i + 1] = charset[((data[j + 0] & 0b00000011) << 4) | (data[j + 1] >> 4)];
        res[i + 2] = charset[((data[j + 1] & 0b00001111) << 2) | (data[j + 2] >> 6)];
        res[i + 3] = charset[data[j + 2] & 0b00111111];
        j += 3;
        i += 4;
    }

    if (len_data - j == 1) {
        res[i + 0] = charset[data[j + 0] >> 2];
        res[i + 1] = charset[((data[j + 0] & 0b00000011) << 4) | (0 >> 4)];
        i += 2;
    } else if (len_data - j == 2) {
        res[i + 0] = charset[data[j + 0] >> 2];
        res[i + 1] = charset[((data[j + 0] & 0b00000011) << 4) | (data[j + 1] >> 4)];
        res[i + 2] = charset[((data[j + 1] & 0b00001111) << 2) | (0 >> 6)];
        i += 3;
    }

    int padding_len = (4 - i) % 4;
    if (padding_len == 1) {
        res[i + 0] = '=';
        i += 1;
    } else if (padding_len == 2) {
        res[i + 0] = '=';
        res[i + 1] = '=';
        i += 2;
    } else if (padding_len == 3) {
        res[i + 0] = '=';
        res[i + 1] = '=';
        res[i + 2] = '=';
        i += 3;
    }
    res[i] = 0;
    printf("%s\n", res);
    return i;
}

int base64_str(const char* s, char* res, size_t res_len) {
    base64(s, strlen(s), res, res_len);
}

int main(void) {
    char res[1000];
    base64_str("Many hands make light work.", res, sizeof res);
    base64_str("Man", res, sizeof res);
    base64_str("Ma", res, sizeof res);
    base64_str("M", res, sizeof res);
}


// with open('./data/favicon-16x16.png', 'rb') as f:
//     data = f.read()
// base64(data)