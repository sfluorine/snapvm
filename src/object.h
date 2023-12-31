#pragma once

#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    OBJ_STRING,
    /* OBJ_ARRAY, */
} object_type_t;

typedef struct {
    object_type_t type;
    bool marked;
    bool freed;
} object_t;

object_t object_make_header(object_type_t);
void object_free(object_t*);

typedef struct {
    object_t header;
    char* ptr;
    size_t size;
} snap_string_t;

object_t* object_make_string(const char*, size_t);
