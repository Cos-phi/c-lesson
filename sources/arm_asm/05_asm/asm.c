#include "clesson.h"

struct Substring {
    char *str;
    int len;
};

int parce_one(char *str, struct Substring* out_subs){
    char* text;
    cl_getline(&text);

    int len = 3;
    out_subs->str = &text[0];
    return len;
}

int asm_one(char* input){
    int word;
    if (0 == strcmp("mov r1, r2" ,input)){
        word = 0xE1A01002;
        return word;
    }else{
        return 0;
    }
}

static void test_asm(){
    char* input = "mov r1, r2";
    int expect = 0xE1A01002; // 1110 00 0 1101 0 0000 0001 00000000 0002
     
    int actual = asm_one(input);

    assert(expect == actual);
}

static void test_parce_one(){
    char* input = "mov r1, r2\n";
    char* expect = "mov";
    
    cl_getline_set_src(input);
    struct Substring actual_sub; 
    int len = parce_one(input, &actual_sub);

    assert(3 == len);
    assert(0 == strncmp(expect, actual_sub.str, len));
}

static void unit_tests(){
    test_asm();
    test_parce_one();
}
int main(){
    unit_tests();
}
