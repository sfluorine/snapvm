#include <stdio.h>
#include <string.h>

#include "vm.h"

typedef struct {
    uint8_t instructions[10000];
    size_t counter;
} program_t;

void program_init(program_t* program)
{
    program->counter = 0;
}

void compile_single_instruction(program_t* program, uint8_t instruction)
{
    memcpy(program->instructions + program->counter, &instruction, 1);
    program->counter += 1;
}

void compile_multiple_instructions(program_t* program, const void* instructions, size_t size)
{
    memcpy(program->instructions + program->counter, instructions, size);
    program->counter += size;
}

void compile_halt(program_t* program)
{
    compile_single_instruction(program, INS_HALT);
}

void compile_scope_enter(program_t* program)
{
    compile_single_instruction(program, INS_SCOPE_ENTER);
}

void compile_scope_leave(program_t* program)
{
    compile_single_instruction(program, INS_SCOPE_LEAVE);
}

void compile_load_object(program_t* program, int64_t index)
{
    compile_single_instruction(program, INS_LOAD_OBJECT);
    compile_multiple_instructions(program, &index, sizeof(index));
}

int main()
{
    snap_vm_t* vm = snap_create();

    program_t program;
    program_init(&program);
    compile_scope_enter(&program);
    compile_load_object(&program, snap_put_object(vm, object_make_string("hehe", 4)));
    compile_load_object(&program, snap_put_object(vm, object_make_string("world", 5)));
    compile_scope_leave(&program);
    compile_halt(&program);

    snap_load_program(vm, program.instructions);
    snap_execute(vm);
    snap_free(vm);
}
