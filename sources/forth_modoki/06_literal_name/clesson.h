#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
return one character and move cursor.
return EOF if end of file.
*/
int cl_getc();
void cl_getc_set_src(char* str);

#ifndef STRUCT_H
#define STRUCT_H

enum LexicalType {
    NUMBER,
    SPACE,
    EXECUTABLE_NAME,
    LITERAL_NAME,
    OPEN_CURLY,
    CLOSE_CURLY, 
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    enum LexicalType ltype;
    union {
        int number;
        char onechar;
        char *name;
    } u;
};

struct KeyValue {
    char *key;
    struct Token value;
};

#endif

#define NAME_SIZE 256
#define STACK_SIZE 1024
int parse_one(int prev_ch, struct Token *out_token); 

void stack_pop(struct Token *out_token);
void stack_push(struct Token *input_token);
int isequal_token(struct Token *token1, struct Token *token2);

int streq(char *s1, char *s2);

void dict_put(char* key, struct Token *token);
int dict_get(char* key, struct Token *out_token);
void dict_print_all();
int isequal_keyvalue(struct KeyValue *keyvalue1, struct KeyValue *keyvalue2);
void unit_tests_dict();
