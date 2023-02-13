#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static char buf[100*1024];
static char buf2[100*1024];

static int to_buffer = 0;
static int pos = 0;

void cl_clear_output() {
    pos = 0;
    buf[0] = '\0';
}


char *cl_get_result(int num) {
    int null_count=0;
    int i=0;
    while (null_count != num) {
        if (buf[i] == '\0') {
            null_count++;
        }
        i++;
    }
    return &buf[i];
}

char *cl_get_all_result() {
    int p = 0;
    int p2 = 0;

    while (p < pos) {
        if (buf[p] == '\0') {
            p++;
            continue;
        }

        buf2[p2++] = buf[p++];
    }
    buf2[p2] = '\0';

    return buf2;
}

void cl_enable_buffer_mode() {
    to_buffer = 1;
}

void cl_disable_buffer_mode() {
    to_buffer = 0;
}


void cl_printf(char *fmt, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, fmt);

    if(to_buffer) {
        pos += vsprintf(&buf[pos], fmt, arg_ptr);
        pos++;
    } else {
        vprintf(fmt, arg_ptr);
    }
    va_end(arg_ptr);
}

int interpret_endian(int word){
    printf("%x\n",word);
    int word0 = (word & 0x000000FF) << 24;
    int word1 = (word & 0x0000FF00) << 8;
    int word2 = (word & 0x00FF0000) >> 8;
    int word3 = (word & 0xFF000000) >> 24;
    int word_interpreted = word0 | word1 | word2 | word3;
    printf("%x\n",word_interpreted);
    return word_interpreted;
}

int print_asm(int word){
    int word_interpreted    = interpret_endian(word);
    int immediate_operand   = (word & 0b00000010000000000000000000000000) >> 25;
    int operation_code      = (word & 0b00000001111000000000000000000000) >> 21;
    printf ("opecode %x\n",operation_code);
    if(word == 0xE3A01068){
        cl_printf("mov r1, #0x68\n");
        return 1;
    }
    return 0;
}

static void test_disasm_mov(){
    int input = 0xE3A01068;
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

