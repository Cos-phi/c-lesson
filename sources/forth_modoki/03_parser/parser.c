#include "clesson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

#define NAME_SIZE 256

enum LexicalType to_lexicaltype(char ch){
    if (ch >= '0' && ch <= '9') return NUMBER;
    else if (ch == ' ') return SPACE;
    else if (ch >= 'A' && ch <= 'z') return EXECUTABLE_NAME;
    else if (ch == '/') return LITERAL_NAME;
    else if (ch == '{') return OPEN_CURLY;
    else if (ch == '}') return CLOSE_CURLY; 
    else if (ch == EOF) return END_OF_FILE;
    else return UNKNOWN;
}

int parse_one(int prev_ch, struct Token *out_token) {
    if(prev_ch == EOF) prev_ch = cl_getc();
    char *cur_name = (char *)malloc(sizeof(char)*NAME_SIZE);
    int cur_number = 0;
    //out_token->u.name = (char *)malloc(sizeof(char)*NAME_SIZE);
    //out_token->u.number = 0;

    enum LexicalType ltype = to_lexicaltype(prev_ch);
    int pos = 0;
    switch( ltype ){
        case EXECUTABLE_NAME:
            do {
                ltype =  to_lexicaltype(prev_ch);
                if( ltype == EXECUTABLE_NAME || ltype == NUMBER){
                    cur_name[pos++] = prev_ch;
                }else{
                    break;
                }
            } while((prev_ch = cl_getc()) != EOF);
            out_token->ltype = EXECUTABLE_NAME;
            out_token->u.name = cur_name;
            return prev_ch;

        case LITERAL_NAME:
            do {
                ltype =  to_lexicaltype(prev_ch);
                if( ltype == LITERAL_NAME){
                    continue;
                }else if( ltype == EXECUTABLE_NAME || ltype == NUMBER){
                    cur_name[pos++] = prev_ch;
                }else{
                    break;
                }
            } while((prev_ch = cl_getc()) != EOF);
            out_token->ltype = LITERAL_NAME;
            out_token->u.name = cur_name;
            return prev_ch;

        case NUMBER:
            do {
                ltype =  to_lexicaltype(prev_ch);
                if( ltype == NUMBER){
                    cur_number *= 10;
                    cur_number += prev_ch - '0';
                }else{
                    break;
                }
            } while((prev_ch = cl_getc()) != EOF);
            out_token->ltype = NUMBER;
            out_token->u.number = cur_number;
            return prev_ch;

        default:
            out_token->ltype = ltype;
            out_token->u.onechar = prev_ch;
            return cl_getc();
    }
}


void parser_print_all() {
    int ch = EOF;
    struct Token token = {
        UNKNOWN,
        {0}
    };

    do {
        ch = parse_one(ch, &token);
        if(token.ltype != UNKNOWN) {
            switch(token.ltype) {
                case NUMBER:
                    printf("num: %d\n", token.u.number);
                    break;
                case SPACE:
                    printf("space!\n");
                    break;
                case OPEN_CURLY:
                    printf("Open curly brace '%c'\n", token.u.onechar);
                    break;
                case CLOSE_CURLY:
                    printf("Close curly brace '%c'\n", token.u.onechar);
                    break;
                case EXECUTABLE_NAME:
                    printf("EXECUTABLE_NAME: %s\n", token.u.name);
                    break;
                case LITERAL_NAME:
                    printf("LITERAL_NAME: %s\n", token.u.name);
                    break;

                default:
                    printf("Unknown type %d\n", token.ltype);
                    break;
            }
        }
    }while(ch != EOF);
}





static void test_parse_one_number() {
    char *input = "123";
    int expect = 123;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);

    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == NUMBER);
    assert(expect == token.u.number);
}

static void test_parse_one_empty_should_return_END_OF_FILE() {
    char *input = "";
    int expect = END_OF_FILE;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == expect);
}


static void unit_tests() {
    test_parse_one_empty_should_return_END_OF_FILE();
    test_parse_one_number();
}

int main() {
    unit_tests();

    cl_getc_set_src("123 45 add /some { 2 3 add } def");
    parser_print_all();
    return 0;
}
