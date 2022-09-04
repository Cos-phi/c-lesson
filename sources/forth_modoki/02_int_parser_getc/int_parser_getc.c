#include "clesson.h"
#include <assert.h>

/*
cc cl_getc.c int_parser_getc.c
*/

enum jikutype {
    NUMBER = 0,
    SPACE  = 1
};

char parse_one(int *out_val, int *out_type) {
    int c;
    int i = 0; 
    while((c = cl_getc()) != EOF) {
        printf("c is %d\n",c);
        enum jikutype out_type_prev = *out_type;
        if (c == 32){
            *out_type = SPACE;
            printf("space\n");
        }else{
            *out_type = NUMBER;
            printf("number\n");
        }
        if (*out_type != out_type_prev) //break;
        printf("%c:type %d\n",c,*out_type);
    }
    out_val[i];
    return *out_val;
}



int main() {
    int answer1 = 0;
    int answer2 = 0;

    // write something here.

    // sample for cl_getc() usage.
    //int c;
    //while((c = cl_getc()) != EOF) {
    //    printf("%c\n",c );
    //}

    printf("aaa\n");
    int out_val;
    int out_type = 1;
    parse_one(&out_val,&out_type);
    printf("outtype:%d\n",out_type);

    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);

    return 0;


}
