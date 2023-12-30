#include <stdio.h>
#include <string.h>

#include "object.h"
#include "vm.h"

int main()
{
    uint8_t program[1024] = {
        INS_SCOPE_ENTER,
        INS_SCOPE_ENTER,
        INS_NEW_OBJ,
    };

    size_t index = 0;
    memcpy(program + 3, &index, 8);

    uint8_t mid[] = {
        INS_NEW_OBJ,
    };
    memcpy(program + 11, mid, 1);

    index = 1;
    memcpy(program + 12, &index, 8);

    uint8_t rest[] = {
        INS_STRING_CONCAT,
        INS_PRINT,
        INS_SCOPE_LEAVE,
        INS_SCOPE_LEAVE,
        INS_HALT,
    };
    memcpy(program + 20, rest, 5);

    svm_t* vm = svm_new();
    svm_load_program(vm, program);
    svm_put_object(vm, object_make_string(make_alloc_string("hello,", 6)));
    svm_put_object(vm, object_make_string(make_alloc_string("world!", 6)));
    svm_launch(vm);
    svm_free(vm);
}
