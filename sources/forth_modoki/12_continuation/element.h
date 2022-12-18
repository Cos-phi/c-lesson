#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#ifndef STRUCT_H
#define STRUCT_H

enum ElementType {
    ELEMENT_NUMBER,
    ELEMENT_LITERAL_NAME,
    ELEMENT_EXECUTABLE_NAME,
    ELEMENT_C_FUNC,
    ELEMENT_EXECUTABLE_ARRAY,
    ELEMENT_UNKNOWN
};

struct Element {
    enum ElementType etype;
    union {
        int number;
        char onechar;
        char *name;
        void (*cfunc)();
        struct ElementArray *byte_codes;
    }u;
};

struct ElementArray {
    int len;
    struct Element elements[0];
};

struct Continuation {
    struct ElementArray *exec_array;
    int pc;
};

#endif

//stack.c
void stack_pop(struct Element *out_element);
void stack_push(struct Element *input_element);
int stack_pop_int();
void stack_print_all();
void stack_clear();

//dict.c
int dict_get(char* key, struct Element *out_element);
void dict_put(char* key, struct Element *element);
void dict_print_all();
void dict_clear();

//hash.c
int hash(char* str);

//continuation_stack
void co_push(struct Continuation *in_cont);
int co_pop(struct Continuation *out_cont);