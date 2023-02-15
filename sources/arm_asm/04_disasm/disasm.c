#include "cl_utils.h"

int interpret_endian(int word){
    //printf("%x\n",word);
    int word0 = (word & 0x000000FF) << 24;
    int word1 = (word & 0x0000FF00) << 8;
    int word2 = (word & 0x00FF0000) >> 8;
    int word3 = (word & 0xFF000000) >> 24;
    int word_interpreted = word0 | word1 | word2 | word3;
    //printf("%x\n",word_interpreted);
    return word_interpreted;
}

int print_asm(int word){
    char output_line[32];
    char *operation_code_str;
    int word_interpreted       = interpret_endian(word);
    int immediate_operand      = (word_interpreted & 0x02000000) >> 25; //00000010000000000000000000000000
    int operation_code         = (word_interpreted & 0x01e00000) >> 21; //00000001111000000000000000000000
    int first_operand_register = (word_interpreted & 0x000f0000) >> 16; //00000000000011110000000000000000
    int destination_register   = (word_interpreted & 0x0000f000) >> 12; //00000000000000001111000000000000
    if( 0xD == operation_code ){
        operation_code_str = "mov";
    }else{
        return 0;
    }
    int immediate_value;
    int second_operand_register;
    if( 1 == immediate_operand ){
        immediate_operand      = (word_interpreted & 0x000000ff); //00000000000000000000000011111111
        sprintf(output_line, "%s r%d, #0x%x\n",operation_code_str, destination_register, immediate_operand); 
        cl_printf(output_line);
        return 1;
    }else{
        second_operand_register= (word_interpreted & 0x0000000f); //00000000000000000000000000001111
        sprintf(output_line, "%s r%d, r%d\n",operation_code_str, destination_register, second_operand_register); 
        return 0;
    }
    //printf ("output_line %s",output_line);
}

static void test_disasm_mov(){
    int input = 0x6810A0E3;
    char* expect = "mov r1, #0x68\n";

    cl_enable_buffer_mode();
    print_asm(input);

    char* actual = cl_get_all_result();
    assert(0 == strcmp(actual,expect));
    cl_clear_output();
}

static void test_disasm_mov_fail(){
    int input = 0x64646464;
    char* expect = "mov r1, #0x68\n";

    cl_enable_buffer_mode();
    print_asm(input);

    char* actual = cl_get_all_result();
    assert(0 == strcmp(actual,expect));
    cl_clear_output();
}

static void unit_tests(){
    test_disasm_mov();
    test_disasm_mov_fail();
}

void main(){
    unit_tests();
}

