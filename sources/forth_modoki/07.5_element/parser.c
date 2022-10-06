#include "parser.h"
#include "clesson.h"


enum LexicalType to_lexicaltype(char ch){
    if (ch >= '0' && ch <= '9') return TOKEN_NUMBER;
    else if (ch == ' ') return TOKEN_SPACE;
    else if (ch >= 'A' && ch <= 'z') return TOKEN_EXECUTABLE_NAME;
    else if (ch == '/') return TOKEN_LITERAL_NAME;
    else if (ch == '{') return TOKEN_OPEN_CURLY;
    else if (ch == '}') return TOKEN_CLOSE_CURLY; 
    else if (ch == EOF) return TOKEN_END_OF_FILE;
    else return TOKEN_UNKNOWN;
}

int parse_one(int prev_ch, struct Token *out_token) {
    if(prev_ch == EOF) prev_ch = cl_getc();
    enum LexicalType ltype = to_lexicaltype(prev_ch);
    int pos = 0;
    switch( ltype ){
        case TOKEN_EXECUTABLE_NAME:
            out_token->ltype = TOKEN_EXECUTABLE_NAME;
            out_token->u.name = (char *)malloc(sizeof(char)*NAME_SIZE);
            do {
                ltype =  to_lexicaltype(prev_ch);
                if( ltype == TOKEN_EXECUTABLE_NAME || ltype == TOKEN_NUMBER){
                    out_token->u.name[pos++] = prev_ch;
                }else{
                    break;
                }
            } while((prev_ch = cl_getc()) != EOF);
            out_token->u.name[pos] = '\0';
            return prev_ch;

        case TOKEN_LITERAL_NAME:
            out_token->ltype = TOKEN_LITERAL_NAME;
            out_token->u.name = (char *)malloc(sizeof(char)*NAME_SIZE);
            do {
                ltype =  to_lexicaltype(prev_ch);
                if( ltype == TOKEN_LITERAL_NAME){
                    continue;
                }else if( ltype == TOKEN_EXECUTABLE_NAME || ltype == TOKEN_NUMBER){
                    out_token->u.name[pos++] = prev_ch;
                }else{
                    break;
                }
            } while((prev_ch = cl_getc()) != EOF);
            out_token->u.name[pos] = '\0';
            return prev_ch;

        case TOKEN_NUMBER:
            out_token->ltype = TOKEN_NUMBER;
            out_token->u.number = 0;
            do {
                ltype =  to_lexicaltype(prev_ch);
                if( ltype == TOKEN_NUMBER){
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
        TOKEN_UNKNOWN,
        {0}
    };

    do {
        ch = parse_one(ch, &token);
        if(token.ltype != TOKEN_UNKNOWN) {
            switch(token.ltype) {
                case TOKEN_NUMBER:
                    printf("num: %d\n", token.u.number);
                    break;
                case TOKEN_SPACE:
                    printf("space!\n");
                    break;
                case TOKEN_OPEN_CURLY:
                    printf("Open curly brace '%c'\n", token.u.onechar);
                    break;
                case TOKEN_CLOSE_CURLY:
                    printf("Close curly brace '%c'\n", token.u.onechar);
                    break;
                case TOKEN_EXECUTABLE_NAME:
                    printf("TOKEN_EXECUTABLE_NAME: %s\n", token.u.name);
                    break;
                case TOKEN_LITERAL_NAME:
                    printf("TOKEN_LITERAL_NAME: %s\n", token.u.name);
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

    struct Token token = {TOKEN_UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);

    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == TOKEN_NUMBER);
    assert(expect == token.u.number);
}

static void test_parse_one_empty_should_return_TOKEN_END_OF_FILE() {
    char *input = "";
    int expect = TOKEN_END_OF_FILE;

    struct Token token = {TOKEN_UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == expect);
}

static void test_parse_one_executable_name(){
    char* input = "add";
    char* expect_name = "add";
    int expect_type = TOKEN_EXECUTABLE_NAME;

    struct Token token = {TOKEN_UNKNOWN, {0}};
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
    int expect_type = TOKEN_LITERAL_NAME;

    struct Token token = {TOKEN_UNKNOWN, {0}};
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
    int expect_type = TOKEN_OPEN_CURLY;

    struct Token token = {TOKEN_UNKNOWN, {0}};
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
    int expect_type = TOKEN_CLOSE_CURLY;

    struct Token token = {TOKEN_UNKNOWN, {0}};
    int ch;

    cl_getc_set_src(input);
    ch = parse_one(EOF, &token);

    assert(ch == EOF);
    assert(token.ltype == expect_type);
    assert(expect_onechar == token.u.onechar);
}

static void unit_tests() {
    test_parse_one_empty_should_return_TOKEN_END_OF_FILE();
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
