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

void init_emitter(struct Emitter* emitter){
    ensure_jit_buf();
    emitter->binary = binary_buf;
    emitter->pos = 0;
}

void emit_MOV_R2_Num(struct Emitter *emitter,int num){
    emitter->binary[emitter->pos++] = 0xe3a02000 | (0x000000FF & num); // mov r2, #num
}
void emit_MVN_R2_Num(struct Emitter *emitter,int num){
    emitter->binary[emitter->pos++] = 0xe3c02000 | (0x000000FF & num); // mvn r2, #num
}
void emit_PUSH_R0(struct Emitter *emitter){
    emitter->binary[emitter->pos++] = 0xE92D0001; // strdb r13!,{r0}
}
void emit_PUSH_R1(struct Emitter *emitter){
    emitter->binary[emitter->pos++] = 0xE92D0002; // strdb r13!,{r1}
}
void emit_PUSH_R2(struct Emitter *emitter){
    emitter->binary[emitter->pos++] = 0xE92D0004; // strdb r13!,{r2}
}
void emit_POP_R0(struct Emitter *emitter){
    emitter->binary[emitter->pos++] = 0xE8BD0001;// ldmia r13!, r0
}
void emit_POP_R2(struct Emitter *emitter){
    emitter->binary[emitter->pos++] = 0xE8BD0004;// ldmia r13!, r2
}
void emit_POP_R3(struct Emitter *emitter){
    emitter->binary[emitter->pos++] = 0xE8BD0008;// ldmia r13!, r2
}
void emit_RETURN_R0(struct Emitter *emitter){
    emitter->binary[emitter->pos++] = 0xe1a0f00e; // mov r15, r14
}
void emit_ADD_R2_R3(struct Emitter *emitter){
    emitter->binary[emitter->pos++] = 0xE0822003; // add r2,r2,r3 
}
void emit_SUB_R2_R3(struct Emitter *emitter){
    emitter->binary[emitter->pos++] = 0xE0422003; // sub r2,r2,r3 
}
void emit_MUL_R2_R3(struct Emitter *emitter){
    emitter->binary[emitter->pos++] = 0xE0020293; // mul r2,r2,r3 
}
void emit_DIV_R2_R3(struct Emitter *emitter){
    /*
    被除数(r2)、除数(r3)、正負の２つの商の候補（r4,r5）について、
     r4 = 0, 1, 2, 3,..
     r5 = 0,-1,-2,-3,..
    と動かしていって
    |被除数 - 候補値×除数| < |除数| のときの候補値が商になります。
    絶対値の比較は、それぞれの2乗を比較することで行います。

    int div(int r2, int r3){
        int ansp = 0; //r4 (anser positve)
        int ansn = 0; //r5 (anser negative)
        while(1){
            ansp++;
            if( (r2-r3*ansp)*(r2-r3*ansp) < r3*r3 ) { 
                return ansp;
            }
            
            ansn--;
            if( (r2-r3*ansn)*(r2-r3*ansn) < r3*r3 ){ 
                return ansn;
            }
        }
    }
    */

    emitter->binary[emitter->pos++] = 0xe3a04000; // mov r4, #0
    emitter->binary[emitter->pos++] = 0xe3a05000; // mov r5, #0
    emitter->binary[emitter->pos++] = 0xe0060393; // mul r6, r3, r3
    emitter->binary[emitter->pos++] = 0xe2844001; // add r4, r4, #1 (loop:)
    emitter->binary[emitter->pos++] = 0xe2455001; // sub r5, r5, #1
    emitter->binary[emitter->pos++] = 0xe0070394; // mul r7, r4, r3
    emitter->binary[emitter->pos++] = 0xe0427007; // sub r7, r2, r7
    emitter->binary[emitter->pos++] = 0xe0080797; // mul r8, r7, r7
    emitter->binary[emitter->pos++] = 0xe1580006; // cmp r8, r6
    emitter->binary[emitter->pos++] = 0xba000005; // blt positive
    emitter->binary[emitter->pos++] = 0xe0070395; // mul r7, r5, r3
    emitter->binary[emitter->pos++] = 0xe0427007; // sub r7, r2, r7
    emitter->binary[emitter->pos++] = 0xe0080797; // mul r8, r7, r7
    emitter->binary[emitter->pos++] = 0xe1580006; // cmp r8, r6
    emitter->binary[emitter->pos++] = 0xba000002; // blt negative
    emitter->binary[emitter->pos++] = 0xeafffff2; // b loop
    emitter->binary[emitter->pos++] = 0xe1a09004; // mov r9, r4 (positive:)
    emitter->binary[emitter->pos++] = 0xea000001; // b end
    emitter->binary[emitter->pos++] = 0xe1a09005; // mov r9, r5 (negative:)
    emitter->binary[emitter->pos++] = 0xeaffffff; // b end
    emitter->binary[emitter->pos++] = 0xe1a02009; // mov r2, r9 (end:)
}

void compile_PUSH_NUM(struct Emitter *emitter,int num){
    if(num >= 0){
        emit_MOV_R2_Num(emitter,num);
    }else{
        emit_MVN_R2_Num(emitter,-1*(num+1));
    }
    emit_PUSH_R2(emitter);
}
void compile_RETURN(struct Emitter *emitter){
    emit_POP_R0(emitter);
    emit_RETURN_R0(emitter);
}
void compile_ADD(struct Emitter *emitter){
    emit_POP_R3(emitter);
    emit_POP_R2(emitter);
    emit_ADD_R2_R3(emitter);
    emit_PUSH_R2(emitter);
}
void compile_SUB(struct Emitter *emitter){
    emit_POP_R3(emitter);
    emit_POP_R2(emitter);
    emit_SUB_R2_R3(emitter);
    emit_PUSH_R2(emitter);
}
void compile_MUL(struct Emitter *emitter){
    emit_POP_R3(emitter);
    emit_POP_R2(emitter);
    emit_MUL_R2_R3(emitter);
    emit_PUSH_R2(emitter);
}
void compile_DIV(struct Emitter *emitter){
    emit_POP_R3(emitter);
    emit_POP_R2(emitter);
    emit_DIV_R2_R3(emitter);
    emit_PUSH_R2(emitter);
}

int compile_token(struct Emitter *emitter,struct Substr *token){
    if( is_number(token->ptr) ) {
        compile_PUSH_NUM(emitter,parse_number(token->ptr));
    }else if( is_register(token->ptr) ){
        if( '1' == token->ptr[1] ){
            emit_PUSH_R1(emitter);
        }else{
            emit_PUSH_R0(emitter);
        }
    }else{
        // must be op.
        switch( parse_word(token) ) {
            case OP_ADD:
                compile_ADD(emitter);
                break;
            case OP_SUB:
                compile_SUB(emitter);
                break;
            case OP_MUL:
                compile_MUL(emitter);                
                break;
            case OP_DIV:
                compile_DIV(emitter);   
                break;
        }
    }
}

int* jit_script(char *input) {
    struct Substr remain_str = {input, strlen(input)};
    struct Emitter binary_emitter;
    init_emitter(&binary_emitter);
    while( !is_end(&remain_str) ){
        skip_space(&remain_str);
        compile_token(&binary_emitter,&remain_str);
        skip_token(&remain_str);
    }
    compile_RETURN(&binary_emitter);
    return binary_emitter.binary;
}

void disasm_binary_buf(){
    for(int i = 0; i < BINARY_BUF_SIZE; i++){
        print_asm(binary_buf[i]);
        if( binary_buf[i] == 0xE1A0F00E ){ // mov r15, r14 (RETURN)
            break;
        }
    }
}

static void test_jit_return_one_value(){
    char* input_script = "3";
    int input_num1 = 10; //dummy num
    int input_num2 = 42; //dummy num
    int expect = 3;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input_script);
    int actual = funcvar(input_num1,input_num2);

    assert_int_eq(expect,actual);
}
static void test_jit_return_one_value_2(){
    char* input_script = "24";
    int input_num1 = 10; //dummy num
    int input_num2 = 42; //dummy num
    int expect = 24;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input_script);
    int actual = funcvar(input_num1,input_num2);

    assert_int_eq(expect,actual);
}
static void test_jit_add(){
    char* input_script = "3 4 add";
    int input_num1 = 10; //dummy num
    int input_num2 = 42; //dummy num
    int expect = 7;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input_script);
    int actual = funcvar(input_num1,input_num2);

    assert_int_eq(expect,actual);
}
static void test_jit_sub(){
    char* input_script = "12 7 sub";
    int input_num1 = 10; //dummy num
    int input_num2 = 42; //dummy num
    int expect = 5;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input_script);
    int actual = funcvar(input_num1,input_num2);
    
    assert_int_eq(expect,actual);
}
static void test_jit_mul(){
    char* input_script = "4 7 mul";
    int input_num1 = 10; //dummy num
    int input_num2 = 42; //dummy num
    int expect = 28;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input_script);
    int actual = funcvar(input_num1,input_num2);
    
    assert_int_eq(expect,actual);
}
static void test_jit_div(){
    char* input_script = "18 3 div";
    int input_num1 = 10; //dummy num
    int input_num2 = 42; //dummy num
    int expect = 6;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input_script);
    int actual = funcvar(input_num1,input_num2);
    
    assert_int_eq(expect,actual);
}
static void test_jit_arg(){
    char* input_script = "r0 r1 add";
    int input_num1 = 10; //arg 1
    int input_num2 = 42; //arg 2
    int expect = 52;

    int (*funcvar)(int, int);
    funcvar = (int(*)(int, int))jit_script(input_script);
    int actual = funcvar(input_num1,input_num2);
    
    assert_int_eq(expect,actual);
}
static void test_jit_variouscase(){
    char* input_script = "-24 r0 div";
    int input_num1 = -8; 
    int input_num2 = 2;
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
    test_jit_return_one_value();
    test_jit_return_one_value_2();
    test_jit_add();
    test_jit_sub();
    test_jit_mul();
    test_jit_div();
    test_jit_arg();
    test_jit_variouscase();
    test_disasm_binary_buf();
    printf("all test done\n");
}

#if 1
int main() {
    int res;
    int (*funcvar)(int, int);

    run_unit_tests();
    run_unit_tests_disasm();
    

    res = eval(1, 5, "3 7 add r1 sub 4 mul");
    //printf("res=%d\n", res);

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
#endif

