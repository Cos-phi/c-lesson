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
int isequal_element(struct Element *element1, struct Element *element2);
void stack_clear();

//dict.c
int streq(char *s1, char *s2);
void dict_put(char* key, struct Element *element);
int dict_get(char* key, struct Element *out_element);
void def();
void dict_print_all();
void dict_clear();
int isequal_keyvalue(struct KeyValue *keyvalue1, struct KeyValue *keyvalue2);
void unit_tests_dict();

//hash.c
int hash(char* str);
