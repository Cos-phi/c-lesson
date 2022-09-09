#include "clesson.h"
#include <assert.h>

/*
cc cl_getc.c int_parser_getc.c
*/

enum tokentype {
    NUMBER = 0,
    SPACE  = 1
};

void cl_getc_set_src();
char parse_one(char c, int *out_val, enum tokentype *out_type); 
void test_parse_one_123(); 
enum tokentype to_tokentype(char c);


void test_parse_one_123() {
    // initialize
    int answer1 = 0;
    int space = 0;
    int answer2 = 0;
    char c = 'S'; 
    enum tokentype out_type;
    cl_getc_set_src("123");

    // run test
    c = parse_one(c,&answer1,&out_type);

    // verity result.
    assert(c == EOF);
    assert(answer1 == 123);
    assert(out_type == NUMBER);
    printf("OK\n");
}

void test_parse_one_123_456() {
    // initialize
    int answer1 = 0;
    int space = 0;
    int answer2 = 0;
    char c = 'S'; 
    enum tokentype out_type;
    cl_getc_set_src("123 456");

    // run test
    c = parse_one(c,&answer1,&out_type);
    c = parse_one(c,&space,&out_type);
    c = parse_one(c,&answer2,&out_type);

    // verity result.
    assert(c == EOF);
    assert(answer1 == 123);
    assert(space == ' ');
    assert(answer2 == 456);
    assert(out_type == NUMBER);
    printf("OK\n");
}

void test_parse_one_ss123() {
    // initialize
    int space = 0;
    int answer1 = 0;
    char c = 'S'; 
    enum tokentype out_type;
    cl_getc_set_src("  123");

    // run test
    c = parse_one(c,&space,&out_type);
    c = parse_one(c,&answer1,&out_type);

    // verity result.
    assert(c == EOF);
    assert(space == ' ');
    assert(answer1 == 123);
    assert(out_type == NUMBER);
    printf("OK\n");
}

int main() {
    int answer1 = 0;
    int space = 0;
    int answer2 = 0;
    char c = 'S';//tart;
    enum tokentype out_type;

    test_parse_one_123();
    test_parse_one_123_456();
    test_parse_one_ss123();

    cl_getc_set_src("123 456");

    c = parse_one(c,&answer1,&out_type);
    c = parse_one(c,&space,&out_type);
    c = parse_one(c,&answer2,&out_type);

    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);

    return 0;


}

enum tokentype to_tokentype(char c){
    if (c == ' ') return SPACE;
    else return NUMBER;
}


char parse_one(char c, int *out_val, enum tokentype *out_type) {
    if( c !=' ' && (c<'0'||c>'9') ) c = cl_getc();
    int cur_val= 0;
    *out_type = to_tokentype(c);

    do{
        enum tokentype cur_type = to_tokentype(c);
        if (*out_type != cur_type) break;
        cur_val = cur_val*10 + c - '0';
    }while((c = cl_getc()) != EOF);

    if (*out_type == NUMBER ) *out_val = cur_val;
    else *out_val = ' ';
    
    return c;
}
