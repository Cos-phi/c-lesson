#include "clesson.h"
#include <assert.h>

/*
cc cl_getc.c int_parser_getc.c
*/

enum jikutype {
    NUMBER = 0,
    SPACE  = 1
};

char parse_one(char c, int *out_val, enum jikutype *out_type) {
    if (c == ' ') *out_type= SPACE;
    else *out_type= NUMBER;
    
    int out_val_tmp = 0;
    do{
        enum jikutype out_type_tmp;
        if (c == ' '){
            out_type_tmp = SPACE;
        }else{
            out_type_tmp = NUMBER;
            out_val_tmp *= 10;
            out_val_tmp += c - '0';
        }
        if (*out_type != out_type_tmp) break;
    }while((c = cl_getc()) != EOF);

    if (*out_type == NUMBER ) {
        *out_val = out_val_tmp;
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
