#include <stddef.h>
#include <stdarg.h>

/**
 * @struct string
 * @brief A dynamically allocated string structure.
 * 
 * This structure represents a resizable string buffer,
 * tracking both the allocated and used sizes.
 * 
 * - start: pointer to the beginning of the character buffer.
 * - available_size: total allocated size in bytes, including already written characters and the final '\0'.
 * - used_size: number of bytes currently used in the buffer, including the final '\0'.
 */
struct string {
    char* start;
    size_t available_size;
    size_t used_size;
};

/**
 * @brief Initializes a string structure with an empty string.
 * 
 * Allocates memory for a string containing only the null terminator.
 * Sets available and used sizes to 1.
 * 
 * @param s Pointer to the string structure to initialize.
 */
void string_init(struct string* s);

/**
 * @brief Frees (de-initializes) memory used by a string structure.
 * 
 * @param s Pointer to the string structure to de-initialize.
 */
void string_deinit(struct string* s);

/**
 * @brief Clears the contents of the string, making it empty.
 *
 * Sets the first character to the null terminator and resets the used size.
 * Does not free or reallocate memory.
 *
 * @param s Pointer to the string structure to clear.
 */
void string_clear(struct string* s);

/**
 * @brief Returns the length of the string (excluding the final '\0').
 * 
 * @param s Pointer to the string structure.
 * 
 * @return The number of characters in the string, excluding the final '\0'.
 */
size_t string_len(struct string* s);

/**
 * @brief Prints the string's content and internal data (pointers and sizes).
 *
 * Outputs the string, its memory address, used size (including null terminator),
 * and available size to stdout.
 *
 * @param s Pointer to the string structure.
 */
void string_print(struct string* s);

/**
 * @brief Append a const char* to the string's content (size of following given).
 * 
 * Reallocates memory when necessary and keeps used and available sizes up to date.
 * 
 * @param s Pointer to the string structure.
 * @param following Pointer to the const char* we want to append at the end of our string.
 * @param len_following Size of following.
 */
void string_append_with_size(struct string* s, const char* following, size_t len_following);

/**
 * @brief Append a const char* to the string's content.
 * 
 * Size of following is computed using strlen!
 * 
 * @param s Pointer to the string structure.
 * @param following Pointer to the const char* we want to append at the end of our string.
 */
void string_append(struct string* s, const char* following);

/**
 * @brief Formats a string and stores the result in a dynamically allocated string struct.
 *
 * This function formats a string using a `printf`-like format string and variadic arguments,
 * allocates memory for the result inside the provided `struct string*`, and stores the formatted
 * string in it.
 *
 * @param s Pointer to the string structure.
 * @param format The format string (as in `printf`).
 * @param ... Variadic arguments to format into the string.
 * 
 * @return The number of characters written (excluding the null terminator),
 *         or a negative value if an error occurred.
 */
int string_snprintf(struct string* s, const char* format, ...);

/**
 * @brief Append a litteral to the string's content.
 *
 * Size of following is computed using sizeof!
 *
 * @param x Pointer to the string structure.
 * @param y String literal to append.
 */
#define string_append_literal(x, y) string_append_with_size(x, y, (sizeof y) - 1)