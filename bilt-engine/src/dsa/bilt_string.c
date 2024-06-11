#include "dsa/bilt_string.h"
#include "main/bilt_memory.h"

#include <string.h>

u64 getLen(const char* str) {
    return strlen(str);
}

char* strDupe(const char* str) {
    u64 length = getLen(str);
    char* copy = mallocate(length + 1, MEM_TYPE_STRING);
    mcopy(copy, str, length + 1);
    return copy;
}

b8 strEq(const char* str1, const char* str2) {
    return strcmp(str1, str2) == 0;
}