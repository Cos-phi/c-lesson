#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef STRUCT_H
#define STRUCT_H

enum ElementType {
    ELEMENT_NUMBER,
    ELEMENT_LITERAL_NAME,
    ELEMENT_UNKNOWN
};

struct Element {
    enum ElementType etype;
    union {
        int number;
        char onechar;
        char *name;
    }u;
};

#endif

//stack.c
void stack_pop(struct Element *out_element);
void stack_push(struct Element *input_element);
void stack_clear();

//dict.c
int dict_get(char* key, struct Element *out_element);
void dict_put(char* key, struct Element *element);
void dict_clear();

//hash.c
int hash(char* str);
