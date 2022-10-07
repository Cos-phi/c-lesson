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
    } u;
};

struct KeyValue {
    char *key;
    struct Element value;
};

#endif

//stack.c
void stack_pop(struct Element *out_element);
void stack_push(struct Element *input_element);
void stack_clear();

//dict.c
void dict_put(char* key, struct Element *element);
int dict_get(char* key, struct Element *out_element);
void def();
void dict_print_all();
void dict_clear();
void unit_tests_dict();

//hash.c
int hash(char* str);
