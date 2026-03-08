
#include "kstring.h"

#include "core/kmemory.h"
#include "string.h"

KAPI u64 string_length(const char* str)
{
    return strlen(str);
}

KAPI char* string_duplicate(const char* str)
{
    u64 length = string_length(str);
    char* new_str = kallocate(length + 1, MEMORY_TAG_STRING);
    kcopy_memory(new_str, str, length+1);
    return new_str;
}

KAPI b8 string_compare(const char* str1, const char* str2)
{
    return strcmp(str1,str2) == 0;
}