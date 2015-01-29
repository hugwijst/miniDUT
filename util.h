#ifndef UTIL_H
#define UTIL_H

#include <string.h>

/**
 * Copy a string, except for the zero termination.
 *
 * @return The destination string, incremented by the length of the source
 * string. This allows for easily repeating calls to copy_str.
 */
inline char* copy_str(char *dest, const char *src) {
    size_t len = strlen(src);
    memcpy(dest, src, len);
    return dest + len;
}

#endif
