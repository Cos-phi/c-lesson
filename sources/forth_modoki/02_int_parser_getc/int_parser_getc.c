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
//        printf("c is %d\n",c);
        enum jikutype out_type_prev = *out_type;
        if (c == 32){
            *out_type = SPACE;
//            printf("space\n");
        }else{
            *out_type = NUMBER;
            *out_val *= 10;
            *out_val += c - 48;
 //           printf("number\n");
        }
        if (*out_type == SPACE && out_type_prev != SPACE) break;
 //       printf("type %d\n",*out_type);
        printf("outval is %d\n",*out_val);
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
    int out_val = 0;
    int out_type = 0;
    int c = parse_one(&out_val,&out_type);
    answer1 = out_val;
    printf("returned c is %d\n",c);
    printf("answer1 is %d\n",answer1);

    out_val = 0;
    out_type = 0;
    c = parse_one(&out_val,&out_type);
    printf("returned c is %d\n",c);
    answer2 = out_val;
    printf("answer2 is %d\n",answer2);

    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);

    return 0;


}
