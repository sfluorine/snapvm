#pragma once

#include "value.h"

#define STACK_CAPACITY 262144
#define OBJECTS_CAPACITY 2048

typedef enum {
    INS_HALT,
    INS_SCOPE_ENTER,
    INS_SCOPE_LEAVE,
    INS_LOAD_OBJECT,
} snap_instruction_t;

typedef struct {
    uint8_t* program;
    int64_t pc; // program counter.

    value_t stack[STACK_CAPACITY];
    int64_t sp; // stack pointer.
    int64_t bp; // base pointer.

    object_t* objects[OBJECTS_CAPACITY];
    int64_t oc; // object counter.

    bool halt;
} snap_vm_t;

snap_vm_t* snap_create();
void snap_free(snap_vm_t*);
void snap_load_program(snap_vm_t*, uint8_t*);
void snap_execute(snap_vm_t*);

int64_t snap_put_object(snap_vm_t*, object_t*);
