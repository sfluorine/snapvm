#include "object.h"

#include <string.h>

object_t object_make_header(object_type_t type)
{
    return (object_t) { type, false, false };
}

object_t* object_make_string(const char* ptr, size_t size)
{
    snap_string_t* string = malloc(sizeof(*string));
    string->header = object_make_header(OBJ_STRING);
    string->ptr = strndup(ptr, size);
    string->size = size;

    return (object_t*)string;
}

void object_free_string(snap_string_t* string)
{
    string->header.freed = 1;
    free(string->ptr);
    
    string->ptr = NULL;
    string->size = 0;
}
