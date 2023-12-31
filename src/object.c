#include "object.h"

#include <string.h>

static void object_free_string(snap_string_t*);

object_t object_make_header(object_type_t type)
{
    return (object_t) { type, false, false };
}

void object_free(object_t* object)
{
    switch (object->type) {
        case OBJ_STRING: {
            object_free_string((snap_string_t*)object);
            break;
        }
    }
}

object_t* object_make_string(const char* ptr, size_t size)
{
    snap_string_t* string = malloc(sizeof(*string));
    string->header = object_make_header(OBJ_STRING);
    string->ptr = strndup(ptr, size);
    string->size = size;

    return (object_t*)string;
}

static void object_free_string(snap_string_t* string)
{
    free(string->ptr);

    string->header.freed = true;
    string->ptr = NULL;
    string->size = 0;
}
