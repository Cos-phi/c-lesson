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
char print_ans(char c, int *cur_val, enum tokentype *cur_type);


void test_parse_one_123() {
    int answer1 = 0;
    int space = 0;
    int answer2 = 0;
    cl_getc_set_src("123");

    char c = cl_getc();
    enum tokentype out_type;
    c = parse_one(c,&answer1,&out_type);

    // verity result.
    assert(c == EOF);
    assert(answer1 == 123);
    assert(out_type == NUMBER);

    printf("OK\n");

}

int main() {
    int answer1 = 0;
    int space = 0;
    int answer2 = 0;

    test_parse_one_123();

    cl_getc_set_src("123 456");
    char c = cl_getc();
    enum tokentype out_type;

    c = parse_one(c,&answer1,&out_type);
    //print_ans(c,&answer1,&out_type);

    c = parse_one(c,&space,&out_type);
    //print_ans(c,&space,&out_type);

    c = parse_one(c,&answer2,&out_type);
    //print_ans(c,&answer2,&out_type);

    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);

    return 0;


}

enum tokentype to_tokentype(char c){
    if (c == ' ') return SPACE;
    else return NUMBER;
}

char print_ans(char c, int *cur_val, enum tokentype *cur_type) {
    printf("c is %c, ",c);
    if (*cur_type == NUMBER ) printf("out_val is %d, ", *cur_val);
    else printf("out_val is \'%c\', ", *cur_val); 
    printf("out_type is %d\n",*cur_type);
}

char parse_one(char c, int *out_val, enum tokentype *out_type) {
    *out_type = to_tokentype(c);
    int cur_val= 0;
    do{
        enum tokentype cur_type = to_tokentype(c);
        if (*out_type != cur_type) break;
        cur_val = cur_val*10 + c - '0';
    }while((c = cl_getc()) != EOF);

    if (*out_type == NUMBER ) {
        *out_val = cur_val;
    }else{
        *out_val = ' ';
    }
    return c;
}
