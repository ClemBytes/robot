#include <stddef.h>

/**
 * @brief Implementation of base64 algorithm for binaries in general.
 * 
 * Binary-to-text encoding into a set of 64 unique characters.
 * Dource is taken 6 bits at a time, in this group of 6 is mapped to one of 64
 * unique characters.
 * See: https://en.wikipedia.org/wiki/Base64
 * 
 * @param data Pointer to the binary to encode.
 * @param len_data Length of binary to encode.
 * @param res Pointer to the result.
 * @param res_len Max-size of result. Needs to be enough.
 * 
 * @return Size of final result. If > to res_len, you have you given enough
 *         space to res.
 */
size_t base64(const char* signed_data, size_t len_data, char* res, size_t res_len);

/**
 * @brief Implementation of base64 algorithm for strings.
 * 
 * Just gets the data length by using strlen().
 * 
 * @param data Pointer to the binary to encode.
 * @param res Pointer to the result.
 * @param res_len Max-size of result. Needs to be enough.
 * 
 * @return Size of final result. If > to res_len, you have you given enough
 *         space to res.
 */
size_t base64_str(const char* s, char* res, size_t res_len);

/**
 * @brief Open and read files.
 * 
 * Deals with errors and allocates required size.
 * 
 * @param path Pointer to the file name.
 * @param file_size_out Pointer to the size of result.
 * 
 * @return Pointer to the result.
 *         Does malloc()! So you should free later.
 */
char* open_and_read(const char* path, size_t* file_size_out);

/**
 * @brief Allocates memory and applies base64 algorithm.
 * 
 * @param data Pointer to the binary to encode.
 * @param size Length of binary to encode.
 * @param res_size_out Pointer to the size of result.
 * 
 * @return Pointer to the result.
 *         Does malloc()! So you should free later.
 */
char* base64_alloc(const char* data, size_t size, size_t* res_size_out);

/**
 * @brief Uses open_and_read() and base64_alloc() to perform base64 algorithm.
 * 
 * @param path Pointer to the binary to encode.
 * @param res_size_out Pointer to the size of result.
 * 
 * @return Pointer to the result.
 *         Does malloc()! So you should free later.
 */
char* base64_from_path(const char* path, size_t* res_size_out);