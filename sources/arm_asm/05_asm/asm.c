#include <stdio.h>
#include <assert.h>

static void test_asm(){
    char* input = "mov r1, r2";
    int expect = 0xE1A01002; // 1110 00 0 1101 0 0000 0001 00000000 0002
     
    //int actual = func(input);

    assert(expect == actual);
}
static void unit_tests(){
    test_asm();
}
int main(){
    unit_tests();
}
