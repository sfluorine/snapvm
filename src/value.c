#include "value.h"

#include <assert.h>
#include <stdio.h>

#define SNAPNAN         0xffff000000000000
#define NANPAYLOAD      0x0000ffffffffffff

#define NULL_MASK       0xfffa000000000000
#define INT_MASK        0xfffb000000000000
#define BOOL_MASK       0xfffc000000000000
#define OBJECT_MASK     0xfffd000000000000

#define IS_DOUBLE(v)    ((v & SNAPNAN) != SNAPNAN)
#define IS_NULL(v)      ((v & SNAPNAN) == NULL_MASK)
#define IS_INT(v)       ((v & SNAPNAN) == INT_MASK)
#define IS_BOOL(v)      ((v & SNAPNAN) == BOOL_MASK)
#define IS_OBJECT(v)    ((v & SNAPNAN) == OBJECT_MASK)

#define OBJECT_UNREACHABLE() \
    {\
        fprintf(stderr, "object unreachable... please update the code accordingly\n");\
        exit(1);\
    }\

value_t value_make_double(double value)
{
    return *(uint64_t*)(&value);
}

value_t value_make_null()
{
    return NULL_MASK;
}

value_t value_make_int(int32_t value)
{
    return (uint64_t)(value) | INT_MASK;
}

value_t value_make_bool(bool value)
{
    return (uint64_t)(value) | BOOL_MASK;
}

value_t value_make_object(object_t* value)
{
    return (uint64_t)(value) | OBJECT_MASK;
}

value_type_t value_get_type(value_t value)
{
    if (IS_NULL(value)) {
        return VAL_NULL;
    } else if (IS_INT(value)) {
        return VAL_INT;
    } else if (IS_BOOL(value)) {
        return VAL_BOOL;
    } else if (IS_OBJECT(value)) {
        return VAL_OBJECT;
    } else if (IS_DOUBLE(value)) {
        return VAL_DOUBLE;
    } else {
        OBJECT_UNREACHABLE();
    }
}

double value_as_double(value_t value)
{
    assert(IS_DOUBLE(value));
    return *(double*)(&value);
}

int32_t value_as_int(value_t value)
{
    assert(IS_INT(value));
    return (int32_t)value;
}

bool value_as_bool(value_t value)
{
    assert(IS_BOOL(value));
    return (bool)(value & 1);
}

object_t* value_as_object(value_t value)
{
    return (object_t*)(value & NANPAYLOAD);
}
