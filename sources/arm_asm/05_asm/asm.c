#include "clesson.h"
#define PARSE_FAIL -1

struct Substring {
    char *str;
    int len;
};

struct Emitter {
    int* words;
    int pos;
};

int array[250]; 
struct Emitter g_emitter = {array,0};

void emit_word(struct Emitter* emitter, int oneword){
    emitter->words[emitter->pos] = oneword;
    emitter->pos++;
}

void hex_dump(struct Emitter* emitter){
    for (int i = 0; i < emitter->pos; i++){
        printf("0x%X\n",emitter->words[i]);
    }
}

int skip_comma(char* str){
    int pos = 0;
    while( ' ' == str[pos] ){ // 先頭の空白は無視
        pos++;
    }
    if( ',' == str[pos] ){
        pos++;
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int parse_register(char* str, int* out_register){
    int pos = 0;
    while( ' ' == str[pos] ){ // 先頭の空白は無視
        pos++;
    }
    if( 'r' == str[pos] ){
        pos++;
        if( '0'==str[pos]||('2'<=str[pos] && '9'>=str[pos]) ){
            *out_register = str[pos]-'0';
            pos++;
        }else if( '1' == str[pos] ){
            pos++;
            if( '0'<=str[pos] && '5'>=str[pos] ){
                *out_register = 10 + str[pos] - '0';
                pos++;
            }else{
                *out_register = 1;
            }
        }else{
            return PARSE_FAIL;
        }
    }else{
        return PARSE_FAIL;
    }
    return pos;
}

int parse_one(char *str, struct Substring* out_subs){
    int pos = 0;
    while( ' ' == str[pos] ){ // 先頭の空白は無視
        pos++;
    }
    if( '\0' == str[pos] ){ // 空白のまま終わった場合
        out_subs->str = str;
        out_subs->len = pos;
        return pos;
    }

    if( ('A'<=str[pos] && 'z'>=str[pos]) || '_'==str[pos] ){
        out_subs->str = &str[pos];
        int start_pos = pos++;
        while( ('0'<=str[pos] && '9'>=str[pos]) || ('A'<=str[pos] && 'z'>=str[pos]) || '_'==str[pos] ){
            pos++;
        } 
        if( (' ' == str[pos]) || (':' == str[pos]) ){
            out_subs->len = (pos - start_pos);
            return pos;
        }else{
            return PARSE_FAIL;
        }
    }else if( ':' == str[pos]){
        out_subs->str = &str[pos];
        pos++;
        out_subs->len = pos;
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int asm_one(char* input){
    struct Substring OpCode; 
    int read_len = parse_one(input, &OpCode);
    input += read_len;

    int Rd; // Destination Register
    read_len = parse_register(input, &Rd);
    input += read_len;
    
    read_len = skip_comma(input);
    input += read_len;

    int Rm; // 2nd operand register
    read_len = parse_register(input, &Rm);
    
    if( 0 == strncmp("mov", OpCode.str, OpCode.len) ){
        int word = 0xE1A00000;
        word |= Rd<<12; 
        word |= Rm;  
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

static void test_parse_one(){
    char* input = "mov r1, r2";
    char* expect_str = "mov";
    int expect_pos = 3;
    int expect_len = 3;
    
    struct Substring actual_sub; 
    int pos = parse_one(input, &actual_sub);

    assert(expect_pos == pos);
    assert(expect_len == actual_sub.len);
    assert(0 == strncmp(expect_str, actual_sub.str, actual_sub.len));
}

static void test_parse_one_indent(){
    char* input = "    mov r1, r2";
    char* expect_str = "mov";
    int expect_len = 3;
    int expect_pos = 7;
    
    struct Substring actual_sub; 
    int pos = parse_one(input, &actual_sub);

    assert(expect_pos == pos);
    assert(expect_len == actual_sub.len);
    assert(0 == strncmp(expect_str, actual_sub.str, actual_sub.len));
}

static void test_parse_one_label(){
    char* input = "  loop:";
    char* expect_str1 = "loop";
    char* expect_str2 = ":";
    
    struct Substring actual_sub1; 
    int read_len = parse_one(input, &actual_sub1);
    input += read_len;
    struct Substring actual_sub2; 
    read_len = parse_one(input, &actual_sub2);

    assert(0 == strncmp(expect_str1, actual_sub1.str, actual_sub1.len));
    assert(0 == strncmp(expect_str2, actual_sub2.str, actual_sub2.len));
}

static void test_parse_one_error(){
    char* input = "abc{}";
    int expect_pos = PARSE_FAIL;
    
    struct Substring actual_sub; 
    int pos = parse_one(input, &actual_sub);

    assert(expect_pos == pos);
}

static void test_parse_one_nothing(){
    char* input = "    ";
    char* expect_str = "    ";
    int expect_len = 4;
    int expect_pos = 4;
    
    struct Substring actual_sub; 
    int pos = parse_one(input, &actual_sub);

    assert(expect_pos == pos);
    assert(expect_len == actual_sub.len);
    assert(0 == strncmp(expect_str, actual_sub.str, actual_sub.len));
}

static void test_parse_register(){
    char* input = "mov r4, r2";
    int expect_r1 = 4;
    
    struct Substring actual_sub; 
    int read_len = parse_one(input, &actual_sub);
    input += read_len;

    int actual_r1;
    read_len = parse_register(input, &actual_r1);
    
    assert(expect_r1 == actual_r1);
    assert(3 == read_len);
}

static void test_parse_register2(){
    char* input = "mov r12, r2";
    int expect_r1 = 12;
    
    struct Substring actual_sub; 
    int read_len = parse_one(input, &actual_sub);
    input += read_len;

    int actual_r1;
    read_len = parse_register(input, &actual_r1);
    
    assert(expect_r1 == actual_r1);
    assert(4 == read_len);
}

static void test_parse_register_and_skip_comma(){
    char* input = "mov r12, r3";
    int expect_r1 = 12;
    int expect_r2 = 3;
    
    struct Substring actual_sub; 
    int read_len = parse_one(input, &actual_sub);
    input += read_len;

    int actual_r1;
    read_len = parse_register(input, &actual_r1);
    input += read_len;
    
    read_len = skip_comma(input);
    input += read_len;

    int actual_r2;
    read_len = parse_register(input, &actual_r2);
    
    assert(expect_r1 == actual_r1);
    assert(expect_r2 == actual_r2);
    assert(3 == read_len);
}

static void unit_tests(){
    test_asm();
    test_parse_one();
    test_parse_one_indent();
    test_parse_one_label();
    test_parse_one_error();
    test_parse_one_nothing();
    test_parse_register();
    test_parse_register2();
    test_parse_register_and_skip_comma();
}

int main(){
    unit_tests();

    char* input = "mov r1, r2\nmov r3, r4\nmov r5, r6\n";
    cl_getline_set_src(input);
    char* buf;
    while( -1 != cl_getline(&buf) ){
        int oneword = asm_one(buf);
        emit_word(&g_emitter, oneword);
    }
    hex_dump(&g_emitter);
    return 0;
}

