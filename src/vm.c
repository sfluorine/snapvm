#include "vm.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t fetch(svm_t*);
static void fetch_arr(svm_t*, uint8_t*, size_t);

static void push_stack(svm_t*, void*);
static void* pop_stack(svm_t*);

static void gc_mark(svm_t*);
static void gc_sweep(svm_t*);

static int is_object(svm_t*, object_t*);

svm_t* svm_new()
{
    svm_t* vm = malloc(sizeof(svm_t));
    if (!vm) {
        return NULL;
    }

    vm->program = NULL;
    vm->pc = 0;

    vm->sp = 0;
    vm->bp = 0;

    vm->oc = 0;

    vm->opc = 0;

    vm->halt = 1;
    return vm;
}

void svm_free(svm_t* vm)
{
    if (vm) {
        for (size_t i = 0; i < vm->oc; i++) {
            object_free(&vm->objects[i]);
        }

        free(vm);
    }
}

void svm_load_program(svm_t* vm, uint8_t* program)
{
    assert(program != NULL);
    vm->program = program;
}

void svm_launch(svm_t* vm)
{
    vm->halt = 0;

    while (!vm->halt) {
        const uint8_t instruction = fetch(vm);

        switch (instruction) {
            case INS_HALT: {
                vm->halt = 1;
                break;
            }
            case INS_NEW_OBJ: {
                uint8_t index_bytes[8];
                fetch_arr(vm, index_bytes, 8);

                size_t index = 0;
                memcpy(&index, index_bytes, 8);

                if (index >= vm->oc) {
                    fprintf(stderr, "ERROR: object index out of bound\n");
                    vm->halt = 1;
                    break;
                }

                // pushing the actual object pointer into the stack.
                size_t sp = vm->sp;
                object_t* object_ptr = &vm->objects[index];
                push_stack(vm, &object_ptr);

                vm->objects_ptrs[vm->opc++] = (object_ptr_t) {.ptr = object_ptr, .sp = sp };

                break;
            }
            case INS_STRING_CONCAT: {
                if (vm->sp - vm->bp < STACK_ELEM_SIZE * 2) {
                    fprintf(stderr, "ERROR: stack underflow\n");
                    vm->halt = 1;
                    break;
                }

                object_t* lhs = *(object_t**)(vm->stack + (vm->sp - STACK_ELEM_SIZE * 2));
                object_t* rhs = *(object_t**)(vm->stack + (vm->sp - STACK_ELEM_SIZE * 1));

                size_t size = lhs->string.size + rhs->string.size;
                char* ptr = malloc(sizeof(char) * size);

                memcpy(ptr, lhs->string.ptr, lhs->string.size);
                memcpy(ptr + lhs->string.size, rhs->string.ptr, rhs->string.size);

                pop_stack(vm);
                pop_stack(vm);

                object_t* object_ptr = &vm->objects[vm->oc];
                object_t concatenated_string = object_make_string(make_string(ptr, size));
                svm_put_object(vm, concatenated_string);

                size_t sp = vm->sp;
                push_stack(vm, &object_ptr);
                vm->objects_ptrs[vm->opc++] = (object_ptr_t) { .ptr = object_ptr, .sp = sp };
                break;
            }
            case INS_PRINT: {
                if (vm->sp - vm->bp < STACK_ELEM_SIZE) {
                    fprintf(stderr, "ERROR: stack underflow\n");
                    vm->halt = 1;
                    break;
                }

                object_t* pot_obj = *(object_t**)(vm->stack + (vm->sp - STACK_ELEM_SIZE * 1));
                if (is_object(vm, pot_obj)) {
                    switch (pot_obj->type) {
                    case OBJ_STRING:
                        printf("%.*s\n", (int)pot_obj->string.size, pot_obj->string.ptr);
                        break;
                    case OBJ_ARRAY:
                        break;
                    }
                }

                pop_stack(vm);
                break;
            }
            case INS_DUP: {
                if (vm->sp - vm->bp < STACK_ELEM_SIZE) {
                    fprintf(stderr, "ERROR: stack underflow\n");
                    vm->halt = 1;
                    break;
                }

                push_stack(vm, vm->stack + vm->sp - STACK_ELEM_SIZE);
                break;
            }
            case INS_SCOPE_ENTER: {
                push_stack(vm, &vm->bp);
                vm->bp = vm->sp;
                break;
            }
            case INS_SCOPE_LEAVE: {
                gc_mark(vm);
                gc_sweep(vm);

                for (size_t i = vm->sp; i > vm->bp; i -= STACK_ELEM_SIZE) {
                    pop_stack(vm);
                }

                memcpy(&vm->bp, pop_stack(vm), STACK_ELEM_SIZE);
                vm->sp = vm->bp;
                break;
            }
            default: {
                fprintf(stderr, "ERROR: unknown instruction: 0x%x\n", instruction);
                vm->halt = 1;
                break;
            }
        }
    }
}

// TODO: implement object free list so the objects that freed are marked free.
// do not rely on objects counter to put objects into the array. it's not scalable.
size_t svm_put_object(svm_t* vm, object_t obj)
{
    assert(vm->oc < OBJECTS_CAP);

    size_t index = vm->oc;
    vm->objects[vm->oc++] = obj;

    return index;
}

static uint8_t fetch(svm_t* vm)
{
    return vm->program[vm->pc++];
}

static void fetch_arr(svm_t* vm, uint8_t* arr, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        arr[i] = fetch(vm);
    }
}

static void push_stack(svm_t* vm, void* ptr)
{
    assert(vm->sp < STACK_CAP);
    assert(ptr != NULL);

    memcpy(vm->stack + vm->sp, ptr, STACK_ELEM_SIZE);
    vm->sp += STACK_ELEM_SIZE;
}

static void* pop_stack(svm_t* vm)
{
    assert(vm->sp > 0);

    vm->sp -= STACK_ELEM_SIZE;
    return vm->stack + vm->sp;
}

static void gc_mark(svm_t* vm)
{
    for (size_t i = 0; i < vm->opc && vm->objects_ptrs[i].sp < vm->sp; i++) {
        vm->objects_ptrs[i].ptr->marked = 1;
    }
}

static void gc_sweep(svm_t* vm)
{
    for (size_t i = 0; i < vm->oc; i++) {
        if (vm->objects[i].marked) {
            vm->objects[i].marked = 0;
        } else {
            object_free(&vm->objects[i]);
        }
    }
}

static int is_object(svm_t* vm, object_t* obj)
{
    for (size_t i = 0; i < vm->opc; i++) {
        if (vm->objects_ptrs[i].ptr == obj) {
            return 1;
        }
    }

    return 0;
}
