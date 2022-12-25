#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef STRUCT_P
#define STRUCT_P

#define NAME_SIZE 256

enum LexicalType {
    TOKEN_NUMBER,
    TOKEN_SPACE,
    TOKEN_EXECUTABLE_NAME,
    TOKEN_LITERAL_NAME,
    TOKEN_OPEN_CURLY,
    TOKEN_CLOSE_CURLY, 
    TOKEN_END_OF_FILE,
    TOKEN_COMMENT,
    TOKEN_UNKNOWN
};

struct Token {
    enum LexicalType ltype;
    union {
        int number;
        char onechar;
        char *name;
    } u;
};
#endif

//cl_getc.c
int cl_getc();
void cl_getc_set_src(char* str);
void cl_getc_set_file(FILE* input_file);

//parce.c
int parse_one(int prev_ch, struct Token *out_token); 
