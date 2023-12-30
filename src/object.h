#pragma once

#include <stddef.h>

typedef struct {
    char* ptr;
    size_t size;
} obj_string_t;

obj_string_t make_string(char*, size_t);
obj_string_t make_alloc_string(const char*, size_t);

typedef struct {
    void* ptr;
    size_t size;
    size_t elem_size;
} obj_array_t;

obj_array_t make_array(const void*, size_t, size_t);

typedef enum {
    OBJ_STRING,
    OBJ_ARRAY,
} object_type_t;

typedef struct {
    object_type_t type;
    union {
        obj_string_t string;
        obj_array_t array;
    };

    int marked;
} object_t;

object_t object_make_string(obj_string_t);
object_t object_make_array(obj_array_t);

void object_free(object_t*);
