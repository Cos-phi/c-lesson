#include "element.h"
#include "clesson.h"

#define CO_STACK_SIZE 1024

static struct Continuation co_stack[CO_STACK_SIZE];
static int co_stack_pos = 0;

void co_push(struct Continuation input_co) {
    co_stack_pos++;
    co_stack[co_stack_pos] = input_co;
}

struct Continuation* co_pop() {
    struct Continuation *out_co;
    if(0 == co_stack_pos){
        out_co->exec_array->len = 0;
        return out_co;
    }
    *out_co = co_stack[co_stack_pos];
    co_stack_pos--;
    return out_co;
}
