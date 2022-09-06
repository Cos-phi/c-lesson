#include "clesson.h"
#include <assert.h>

/*
cc cl_getc.c int_parser_getc.c
*/

enum tokentype {
    NUMBER = 0,
    SPACE  = 1
};

enum tokentype to_tokentype(char c){
    if (c == ' ') return SPACE;
    else return NUMBER;
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



int main() {
    int answer1 = 0;
    int space = 0;
    int answer2 = 0;

    cl_getc_set_src("123   456");

    char c = cl_getc();
    enum tokentype out_type;

    c = parse_one(c,&answer1,&out_type);
    printf("parse_one\nc is %c, out_val is %d, out_type is %d\n\n",c,answer1,out_type);
    c = parse_one(c,&space,&out_type);
    printf("parse_one\nc is %c, out_val is %c, out_type is %d\n\n",c,space,out_type);
    c = parse_one(c,&answer2,&out_type);
    printf("parse_one\nc is %c, out_val is %d, out_type is %d\n\n",c,answer2,out_type);



    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);

    return 0;


}
