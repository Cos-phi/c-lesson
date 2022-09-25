#include "clesson.h"

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
    enum LexicalType ltype = to_lexicaltype(prev_ch);
    int pos = 0;
    switch( ltype ){
        case EXECUTABLE_NAME:
            out_token->ltype = EXECUTABLE_NAME;
            out_token->u.name = (char *)malloc(sizeof(char)*NAME_SIZE);
            do {
                ltype =  to_lexicaltype(prev_ch);
                if( ltype == EXECUTABLE_NAME || ltype == NUMBER){
                    out_token->u.name[pos++] = prev_ch;
                }else{
                    break;
                }
            } while((prev_ch = cl_getc()) != EOF);
            out_token->u.name[pos] = '\0';
            return prev_ch;

        case LITERAL_NAME:
            out_token->ltype = LITERAL_NAME;
            out_token->u.name = (char *)malloc(sizeof(char)*NAME_SIZE);
            do {
                ltype =  to_lexicaltype(prev_ch);
                if( ltype == LITERAL_NAME){
                    continue;
                }else if( ltype == EXECUTABLE_NAME || ltype == NUMBER){
                    out_token->u.name[pos++] = prev_ch;
                }else{
                    break;
                }
            } while((prev_ch = cl_getc()) != EOF);
            out_token->u.name[pos] = '\0';
            return prev_ch;

        case NUMBER:
            out_token->ltype = NUMBER;
            out_token->u.number = 0;
            do {
                ltype =  to_lexicaltype(prev_ch);
                if( ltype == NUMBER){
                    out_token->u.number *= 10;
                    out_token->u.number += prev_ch - '0';
                }else{
                    break;
                }
            } while((prev_ch = cl_getc()) != EOF);
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

static void test_parse_one_executable_name(){
    char* input = "add";
    char* expect_name = "add";
    int expect_type = EXECUTABLE_NAME;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == expect_type);
    assert(0 == strcmp(expect_name,token.u.name));
}

static void test_parse_one_literal_name(){
    char* input = "/add";
    char* expect_name = "add";
    int expect_type = LITERAL_NAME;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == expect_type);
    assert(0 == strcmp(expect_name,token.u.name));
}

static void test_parse_one_open_curly(){
    char* input = "{";
    char expect_onechar = '{';
    int expect_type = OPEN_CURLY;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == expect_type);
    assert(expect_onechar == token.u.onechar);
}

static void test_parse_one_close_curly(){
    char* input = "}";
    char expect_onechar = '}';
    int expect_type = CLOSE_CURLY;

    struct Token token = {UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == expect_type);
    assert(expect_onechar == token.u.onechar);
}

static void unit_tests() {
    test_parse_one_empty_should_return_END_OF_FILE();
    test_parse_one_number();
    test_parse_one_executable_name();
    test_parse_one_literal_name();
    test_parse_one_open_curly();
    test_parse_one_close_curly();
}
#if 0
int main() {
    unit_tests();

    cl_getc_set_src("123 45 add /some { 2 3 add } def");
    parser_print_all();
    return 0;
}
#endif
