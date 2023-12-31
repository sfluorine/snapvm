#include "vm.h"

#include <stdio.h>
#include <string.h>

static uint8_t fetch(snap_vm_t*);
static void fetch_array(snap_vm_t*, uint8_t*, size_t);

static void push_stack(snap_vm_t*, value_t);
static value_t pop_stack(snap_vm_t*);

snap_vm_t* snap_create()
{
    snap_vm_t* vm = malloc(sizeof(*vm));
    vm->pc = 0;
    vm->sp = 0;
    vm->bp = 0;
    vm->oc = 0;
    vm->halt = true;

    return vm;
}

void snap_free(snap_vm_t* vm)
{
    for (int64_t i = 0; i < vm->oc; i++) {
        if (vm->objects[i] != NULL && !vm->objects[i]->freed) {
            object_free(vm->objects[i]);

            free(vm->objects[i]);
            vm->objects[i] = NULL;
        }
    }

    free(vm);
}

void snap_load_program(snap_vm_t* vm, uint8_t* program)
{
    vm->program = program;
    vm->halt = false;
}

int64_t snap_put_object(snap_vm_t* vm, object_t* object)
{
    if (vm->oc >= OBJECTS_CAPACITY) {
        fprintf(stderr, "ERROR: objects overflow\n");
        vm->halt = true;
        return -1;
    }

    int64_t index = vm->oc;

    for (int64_t i = 0; i < vm->oc; i++) {
        object_t* obj = vm->objects[i];

        if (obj != NULL && obj->freed) {
            free(obj);
            vm->objects[i] = object;
            return i;
        }

    }

    vm->objects[vm->oc++] = object;
    return index;
}

void snap_execute(snap_vm_t* vm)
{
    while (!vm->halt) {
        uint8_t instruction = fetch(vm);

        switch (instruction) {
            case INS_HALT: {
                vm->halt = true;
                break;
            }
            case INS_SCOPE_ENTER: {
                push_stack(vm, value_make_int(vm->bp));
                if (vm->halt) {
                    break;
                }

                vm->bp = vm->sp;
                break;
            }
            case INS_SCOPE_LEAVE: {
                vm->sp = vm->bp;

                value_t bp = pop_stack(vm);
                if (vm->halt) {
                    break;
                }

                vm->bp = value_as_int(bp);
                break;
            }
            case INS_LOAD_OBJECT: {
                int64_t index = 0;

                uint8_t index_bytes[sizeof(index)];
                fetch_array(vm, index_bytes, sizeof(index));

                memcpy(&index, index_bytes, sizeof(index));

                if (index >= vm->oc) {
                    fprintf(stderr, "ERROR: invalid object index\n");
                    vm->halt = true;
                    break;
                }

                push_stack(vm, value_make_object(vm->objects[index]));
                break;
            }
            default: {
                fprintf(stderr, "ERROR: unknown instruction: 0x%x\n", instruction);
                vm->halt = true;
                break;
            }
        }
    }
}

static uint8_t fetch(snap_vm_t* vm)
{
    return vm->program[vm->pc++];
}

static void fetch_array(snap_vm_t* vm, uint8_t* array, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        array[i] = fetch(vm);
    }
}

static void push_stack(snap_vm_t* vm, value_t value)
{
    if (vm->sp >= STACK_CAPACITY) {
        fprintf(stderr, "ERROR: stack overflow\n");
        vm->halt = true;
        return;
    }

    vm->stack[vm->sp++] = value;
}

static value_t pop_stack(snap_vm_t* vm)
{
    if (vm->sp - vm->bp < 0) {
        fprintf(stderr, "ERROR: stack underflow\n");
        vm->halt = true;
        return value_make_null();
    }

    return vm->stack[--vm->sp];
}
