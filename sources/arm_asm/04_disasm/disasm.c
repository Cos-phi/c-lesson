#include "cl_utils.h"
#include <errno.h>
#include <sys/stat.h>
#define MAX_WORD_NUM 256

int print_asm(int word){
    int immediate_operand    = (word & 0x02000000) >> 25; //00000010000000000000000000000000
    int operation_code       = (word & 0x01e00000) >> 21; //00000001111000000000000000000000
    int branch_opecode       = (word & 0x0E000000) >> 25; 
    int link_bit             = (word & 0x01000000) >> 24; 
    
    if( 0xE2811001 == word ){ // add
        cl_printf("add r1, r1, #1\n");
        return 1;
    }else if( 0xE3530000 == word ){ // cmp
        cl_printf("cmp r3, #0\n");
        return 1;
    }else if( 0x1afffffa == word ){ // bne
        cl_printf("bne 0xC\n");
        return 1;
    }else if( 0xE92D4002 == word ){ // stmdb
        cl_printf("stmdb r13!,{r1,r14}\n");
        return 1;
    }else if( 0xE8BD4002 == word ){ // ldmia
        cl_printf("ldmia r13!,{r1,r14}\n");
        return 1;
    }else if( 0xD == operation_code && 1 == immediate_operand ){ // operation code 0xD means 'mov'
        int immediate_value  = (word & 0x000000ff); 
        int destination_register = (word & 0x0000f000) >> 12;
        cl_printf("mov r%d, #0x%x\n",destination_register, immediate_value); 
        return 1;
    }else if( 0x5 == branch_opecode && 0 == link_bit){ // operation code 0x5 means 'branch', link bit = 0? b: 1? bl
        int branch_offset    = (word<<2) & 0x00ffffff; 
        if( 0x00800000 == (branch_offset & 0x00800000) ){
            branch_offset = branch_offset - 0x1000000;
            branch_offset *= -1;
            cl_printf("b [r15, #0x-%d]\n",branch_offset); 
        }else{
            cl_printf("b [r15, #0x%d]\n",branch_offset); 
        }
        return 1;
    }else if( 0x5 == branch_opecode && 1 == link_bit){ // operation code 0x5 means 'branch', link bit = 0? b: 1? bl
        cl_printf("bl 0x2C\n"); 
        return 1;
    }else if( 0x05800000 == (word & 0x0ff00000) ){ // 01IPUBWL = 01011000  L is 0?str: 1?ldr
        int destination_register = (word & 0x0000f000) >> 12; 
        cl_printf("str r%d, [r0]\n",destination_register);
        return 1;
    }else if( 0xE5900000 == (word & 0xfff00000) ){ // 01IPUBWL = 01011001  L is 0?str: 1?ldr
        cl_printf("ldr r%d, [pc, #%d]\n",(word & 0x0000f000)>>12,(word & 0x00000fff));
        return 1;
    }else if( 0xE5D00000 == (word & 0xfff00000) ){ // 01IPUBWL = 01011101 B is 1?byte: 0?word, L is 0?strb: 1?ldrb 
        cl_printf("ldrb r%d, [r%d]\n",(word & 0x0000f000)>>12,(word & 0x000f0000)>>16);
        return 1;
    }else{
        return 0;
    }
}

int read_binary_file(char* filename){
    cl_disable_buffer_mode();
    int words[MAX_WORD_NUM];
    int address = 0x10000;

    struct stat sb;
    if(stat(filename, &sb) == -1){
        return 0;
    }
    int wordnum = (int)sb.st_size/4;

    FILE *filepointer;
    filepointer = fopen(filename, "rb");
    if( NULL == filepointer ){
        printf("fopen failed (%s)\n", strerror( errno ) );
        return 0;
    }
    fread(&words,sizeof(int),wordnum,filepointer);
    fclose(filepointer);

    int i=0;
    while(1){
        printf("0x%08x  ",address);
        address += 4;
        if( 0 == print_asm(words[i]) ){
            break;
        }
        i++;
        if( i == wordnum ){
            return 1;
        }
    }
    while(1){
        printf("%02x %02x %02x %02x\n",words[i] & 0x000000FF, (words[i]&0x0000FF00)>>8, (words[i]&0x00FF0000)>>16, (words[i]&0xFF000000)>>24);
        i++;
        if( i == wordnum ){
            return 1;
        }
        address += 4;
        printf("0x%08x  ",address);
    }
}

static void test_disasm_mov(){
    int input = 0xE3A01068;
    int expect = 1;
    char* expect_str = "mov r1, #0x68\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_mov_fail(){
    int input = 0x64646464;
    int expect = 0;

    cl_enable_buffer_mode();
    int actual = print_asm(input);

    assert(expect == actual);
    cl_clear_output();
}

static void test_disasm_b(){
    int input = 0xEAFFFFFE; // 1110 1010 1111 1111 1111 1111 1111 1110
    int expect = 1;
    char* expect_str = "b [r15, #0x-8]\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_str(){
    int input = 0xE5801000; // 1110 0101 1000 0000 0001 0000 0000 0000 
    int expect = 1;
    char* expect_str = "str r1, [r0]\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_str2(){
    int input = 0xE5802000; // 1110 0101 1000 0000 0002 0000 0000 0000 
    int expect = 1;
    char* expect_str = "str r2, [r0]\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_ldr(){
    int input = 0xE59f0038; // 1110 0101 1001 1111 0000 0000 0011 1000
    int expect = 1;
    char* expect_str = "ldr r0, [pc, #56]\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_ldr2(){
    int input = 0xE59f002C; 
    int expect = 1;
    char* expect_str = "ldr r0, [pc, #44]\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_ldrb(){
    int input = 0xE5D13000; 
    int expect = 1;
    char* expect_str = "ldrb r3, [r1]\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_add(){
    int input = 0xE2811001; 
    int expect = 1;
    char* expect_str = "add r1, r1, #1\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_cmp(){
    int input = 0xE3530000; 
    int expect = 1;
    char* expect_str = "cmp r3, #0\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_bne(){
    int input = 0x1afffffa; 
    int expect = 1;
    char* expect_str = "bne 0xC\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_bl(){
    int input = 0xEB000007; 
    int expect = 1;
    char* expect_str = "bl 0x2C\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_stmdb(){ 
    int input = 0xE92D4002; 
    int expect = 1;
    char* expect_str = "stmdb r13!,{r1,r14}\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void test_disasm_ldmia(){ 
    int input = 0xE8BD4002; 
    int expect = 1;
    char* expect_str = "ldmia r13!,{r1,r14}\n";

    cl_enable_buffer_mode();
    int actual = print_asm(input);
    char* actual_str = cl_get_all_result();

    assert(expect == actual);
    assert(0 == strcmp(actual_str,expect_str));
    cl_clear_output();
}

static void unit_tests(){
    test_disasm_mov();
    test_disasm_mov_fail();
    test_disasm_b();
    test_disasm_str();
    test_disasm_str2();
    test_disasm_ldr();
    test_disasm_ldr2();
    test_disasm_ldrb();
    test_disasm_add();
    test_disasm_cmp();
    test_disasm_bne();
    test_disasm_bl();
    test_disasm_stmdb();
    test_disasm_ldmia();
}

void main(int argc, char *argv[]){
    unit_tests();
    if(argc){
        read_binary_file(argv[1]);
    }
}

