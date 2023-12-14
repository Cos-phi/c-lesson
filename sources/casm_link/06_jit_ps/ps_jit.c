#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "parser.h"
#include "test_util.h"
#include "disasm.h"
#include "cl_utils.h"

extern int eval(int r0, int r1, char *str);

/*
JIT
*/
int *binary_buf = NULL;

int* allocate_executable_buf(int size) {
    return (int*)mmap(0, size,
                 PROT_READ | PROT_WRITE | PROT_EXEC,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void ensure_jit_buf() {
    if(binary_buf == NULL) {
        binary_buf = allocate_executable_buf(1024);
    }
}

int* jit_script(char *input) {
    ensure_jit_buf();
    /*
    TODO: emit binary here
    */
    // dummy code to avoid crash.
    binary_buf[0] = 0xe3a00003; // mov r0, #3
    binary_buf[1] = 0xe1a0f00e; // mov r15, r14

    return binary_buf;
}

void disasm_binary_buf(){
    for(int i = 0; binary_buf[i] != 0xE1A0F00E; i++){
        print_asm(binary_buf[i]);
    }
    
}

static void test_jit_hardcode_return_value(){
    char* input_script = "dummy";
    int input_num1 = 10;
    int input_num2 = 42;
    int expect = 3;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script("dummy");
    int actual = funcvar(input_num1,input_num2);

    assert_int_eq(expect,actual);
}

static void test_disasm_binary_buf(){
    char* input_script = "dummy";
    char* expect_str = "mov r0, #0x3\n";

    jit_script("dummy");
    cl_enable_buffer_mode();
    disasm_binary_buf();
    char* actual_str = cl_get_all_result();

    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void run_unit_tests() {
    test_jit_hardcode_return_value();
    test_disasm_binary_buf();
    printf("all test done\n");
}


int main() {
    int res;
    int (*funcvar)(int, int);

    run_unit_tests();
    run_unit_tests_disasm();
    

    res = eval(1, 5, "3 7 add r1 sub 4 mul");
    printf("res=%d\n", res);

    /*
     TODO: Make below test pass.
    */
    funcvar = (int(*)(int, int))jit_script("3 7 add r1 sub 4 mul");

    res = funcvar(1, 5);
    assert_int_eq(20, res);

    res = funcvar(1, 4);
    assert_int_eq(24, res);

    return 0;
}

