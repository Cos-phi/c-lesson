#include "clesson.h"
#include <assert.h>

/*
cc cl_getc.c int_parser_getc.c
*/

enum jikutype {
    NUMBER = 0,
    SPACE  = 1
};

enum jikutype to_jikutype(char c){
    if (c == ' ') return SPACE;
    else return NUMBER;
}

char parse_one(char c, int *out_val, enum jikutype *out_type) {
    *out_type = to_jikutype(c);
    int tmp_out_val= 0;
    do{
        enum jikutype tmp_out_type = to_jikutype(c);
        if (*out_type != tmp_out_type) break;
        tmp_out_val *= 10;
        tmp_out_val += c - '0';
    }while((c = cl_getc()) != EOF);

    if (*out_type == NUMBER ) {
        *out_val = tmp_out_val;
    }else{
        *out_val = ' ';
    }
    return c;
}



int main() {
    int answer1 = 0;
    int space = 0;
    int answer2 = 0;

    // write something here.

    // sample for cl_getc() usage.
    //int c;
    //while((c = cl_getc()) != EOF) {
    //    printf("%c\n",c );
    //}

    char c = cl_getc();
    enum jikutype out_type;


    c = parse_one(c,&answer1,&out_type);
    printf("parse_one\n");
    printf("c is %c, out_val is %d, out_type is %d\n\n",c,answer1,out_type);

    c = parse_one(c,&space,&out_type);
    printf("parse_one\n");
    printf("c is %c, out_val is %c, out_type is %d\n\n",c,space,out_type);

    c = parse_one(c,&answer2,&out_type);
    printf("parse_one\n");
    printf("c is %c, out_val is %d, out_type is %d\n\n",c,answer2,out_type);


    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);

    return 0;


}
