#pragma once

#include <stdint.h>
#include <stddef.h>

#include "object.h"

#define STACK_CAP 2097152
#define OBJECTS_CAP 1024
#define STACK_ELEM_SIZE 8

typedef struct {
    object_t* ptr;
    size_t sp;
} object_ptr_t;

typedef enum {
    INS_HALT,
    INS_NEW_OBJ,
    INS_STRING_CONCAT,
    INS_PRINT,
    INS_DUP,
    INS_SCOPE_ENTER,
    INS_SCOPE_LEAVE,
} snap_instruction_t;

typedef struct {
    uint8_t* program;
    size_t pc; // program counter.

    uint8_t stack[STACK_CAP];
    size_t sp; // stack pointer.
    size_t bp; // base pointer.

    object_t objects[OBJECTS_CAP];
    size_t oc; // objects counter.

    object_ptr_t objects_ptrs[OBJECTS_CAP];
    size_t opc; // objects pointers counter.

    int halt;
} svm_t;

svm_t* svm_new();
void svm_free(svm_t*);
void svm_load_program(svm_t* , uint8_t*);
void svm_launch(svm_t*);

size_t svm_put_object(svm_t*, object_t);
