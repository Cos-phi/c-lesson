#include "cl_utils.h"

int print_asm(int word){
    int immediate_operand    = (word & 0x02000000) >> 25; //00000010000000000000000000000000
    int operation_code       = (word & 0x01e00000) >> 21; //00000001111000000000000000000000
    int destination_register = (word & 0x0000f000) >> 12; //00000000000000001111000000000000
         
                                     //0x6   4   6..
                                     //0b011001000110..
                                     //0xE   A   F   F   F   F   F   E
                                     //0b11101010111111111111111111111110
    int branch               = (word & 0b00001110000000000000000000000000) >> 25; 
    int link_bit             = (word & 0b00000001000000000000000000000000) >> 24; 

    if( 0xD == operation_code && 1 == immediate_operand ){ 
        int immediate_value  = (word & 0x000000ff); 
        cl_printf("mov r%d, #0x%x\n",destination_register, immediate_value); 
        return 1;
    }else if( 0x5 == branch && 0 == link_bit){
        cl_printf("b [r15, #-0x8]\n"); 
        return 1;

    }else{
        return 0;
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
    int input = 0xEAFFFFFE; // 1110 1010 1111 1111 1111 1111 1111 1111 1110
    int expect = 1;
    char* expect_str = "b [r15, #-0x8]\n";

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
}

void main(){
    unit_tests();
}

