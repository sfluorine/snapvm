#pragma once

#include "object.h"

#include <stdint.h>

typedef uint64_t value_t;

typedef enum {
    VAL_DOUBLE,
    VAL_NULL,
    VAL_INT,
    VAL_BOOL,
    VAL_OBJECT,
} value_type_t;

value_t value_make_double(double);
value_t value_make_null();
value_t value_make_int(int32_t);
value_t value_make_bool(bool);
value_t value_make_object(object_t*);

value_type_t value_get_type(value_t);

double value_as_double(value_t);
int32_t value_as_int(value_t);
bool value_as_bool(value_t);
object_t* value_as_object(value_t);
