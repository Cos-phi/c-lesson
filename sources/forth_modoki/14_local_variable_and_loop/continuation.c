#include "element.h"
#include "clesson.h"

#define CO_STACK_SIZE 1024

static struct CoStackElement co_stack[CO_STACK_SIZE];
static int co_stack_pos = 0;

void co_push(struct CoStackElement *in_cont) {
    co_stack_pos++;
    co_stack[co_stack_pos] = *in_cont;
}

int co_pop(struct CoStackElement *out_cont) {
    if(0 == co_stack_pos){
        return 0;
    }
    *out_cont = co_stack[co_stack_pos];
    co_stack_pos--;
    return 1;
}
