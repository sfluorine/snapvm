#include "object.h"

#include <stdlib.h>
#include <string.h>

obj_string_t make_string(char* ptr, size_t size)
{
    return (obj_string_t) { ptr, size };
}

obj_string_t make_alloc_string(const char* str, size_t size)
{
    char* ptr = strndup(str, size);
    return (obj_string_t) { ptr, size };
}

obj_array_t make_array(const void* ptr, size_t size, size_t elem_size)
{
    void* new_ptr = malloc(size);
    memcpy(new_ptr, ptr, size);

    return (obj_array_t) { new_ptr, size, elem_size };
}

object_t object_make_string(obj_string_t str)
{
    object_t obj;
    obj.type = OBJ_STRING;
    obj.string = str;
    obj.marked = 0;

    return obj;
}

object_t object_make_array(obj_array_t arr)
{
    object_t obj;
    obj.type = OBJ_ARRAY;
    obj.array = arr;
    obj.marked = 0;

    return obj;
}

void object_free(object_t* obj)
{
    switch (obj->type) {
        case OBJ_STRING: {
            obj_string_t* str = &obj->string;
            free(str->ptr);

            str->ptr = NULL;
            str->size = 0;
            break;
        }
        case OBJ_ARRAY: {
            obj_array_t* arr = &obj->array;
            free(arr->ptr);

            arr->ptr = NULL;
            arr->size = 0;
            break;
        }
    }
}
