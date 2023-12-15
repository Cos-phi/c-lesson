#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "parser.h"
#include "test_util.h"
#include "disasm.h"
#include "cl_utils.h"


#define BINARY_BUF_SIZE 1024

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
        binary_buf = allocate_executable_buf(BINARY_BUF_SIZE);
    }
}

/*
Emitter
*/
struct Emitter {
  int *binary;
  int pos;
};

void emit_MOV_R2_Num3(struct Emitter *emitter, int binary) {
    emitter->binary[emitter->pos++] = 0xe3a02003;
}

void emit_PUSH_R2(struct Emitter *emitter) {
    emitter->binary[emitter->pos++] = 0xE92D0004; // strdb r13!,{r2}
}

void emit_POP_R0(struct Emitter *emitter) {
    emitter->binary[emitter->pos++] = 0xE8BD0001;// ldmia r13!, r0
}

void emit_RETURN(struct Emitter *emitter) {
    emitter->binary[emitter->pos++] = 0xe1a0f00e; // mov r15, r14
}

int* jit_script(char *input) {
    ensure_jit_buf();
    int buf_pos = 0;
    /*
    TODO: emit binary here
    */
    
    binary_buf[buf_pos++] = 0xe3a02003; // mov r2, #3
    binary_buf[buf_pos++] = 0xE92D0004; // strdb r13!,{r2}

    binary_buf[buf_pos++] = 0xE8BD0001;// ldmia r13!, r0
    binary_buf[buf_pos] = 0xe1a0f00e; // mov r15, r14

    return binary_buf;
}

void disasm_binary_buf(){
    for(int i = 0; i < BINARY_BUF_SIZE; i++){
        print_asm(binary_buf[i]);
        if(binary_buf[i] == 0xE1A0F00E){
            break;
        }
    }
    
}

static void test_jit_hardcode_return_value(){
    char* input_script = "3";
    int input_num1 = 10; //dummy num
    int input_num2 = 42; //dummy num
    int expect = 3;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input_script);
    int actual = funcvar(input_num1,input_num2);

    assert_int_eq(expect,actual);
}

static void test_disasm_binary_buf(){
    char* input_script = "3";
    char* expect_str = "mov r2, #0x3\nstmdb r13!,{r2}\nldmia r13!,{r0}\nmov r15, r14\n";

    jit_script(input_script);
    cl_enable_buffer_mode();
    disasm_binary_buf();
    char* actual_str = cl_get_all_result();

    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
    cl_disable_buffer_mode();
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

