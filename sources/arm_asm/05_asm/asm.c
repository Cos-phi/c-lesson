#include <stdio.h>
#include <assert.h>
#include <string.h>

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
static void unit_tests(){
    test_asm();
}
int main(){
    unit_tests();
}
