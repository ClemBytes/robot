#include <stddef.h>

size_t base64(const unsigned char* data, size_t len_data, char* res, size_t res_len);
size_t base64_str(const char* s, char* res, size_t res_len);
char* open_and_read(const char* path, size_t* file_size_out);
char* base64_alloc(const char* data, size_t size, size_t* res_size_out);
char* base64_from_path(const char* path, size_t* res_size_out);