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
    ELEMENT_OPERATOR,
    ELEMENT_EXECUTABLE_ARRAY,
    ELEMENT_UNKNOWN
};

enum ControlOperator {
    OP_EXEC,
    OP_JMP,
    OP_JMP_NOT_IF,
};

struct Element {
    enum ElementType etype;
    union {
        int number;
        char onechar;
        char *name;
        void (*cfunc)();
        void (*emitter_cfunc)(struct Emitter*);
        struct ElementArray *byte_codes;
        enum ControlOperator op;
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

struct Emitter {
    struct Element *elems;    
    int pos;
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
int compile_dict_get(char* key, struct Element *out_element);
void compile_dict_put(char* key, struct Element *element);
void dict_put(char* key, struct Element *element);
void dict_print_all();
void dict_clear();
int streq(char *s1,char *s2);

//hash.c
int hash(char* str);

//continuation_stack
void co_push(struct Continuation *in_cont);
int co_pop(struct Continuation *out_cont);