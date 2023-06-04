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

int is_sbracket(char* str){
    int pos = 0;
    while( ' ' == str[pos] ){ // 先頭の空白は無視
        pos++;
    }

    if( ('[' == str[pos])||(']' == str[pos]) ){
        return 1;
    }else{
        return 0;
    }
}

int parse_raw_value(char* str, int* out_value){
    int pos = 0;
    *out_value = 0;
    while( ' ' == str[pos] ){ // 先頭の空白は無視
        pos++;
    }
    if( ('0' == str[pos++]) && ('x' == str[pos++]) ){ 
        while(1){
            if( (str[pos] >= '0')&&(str[pos] <= '9') ){
                *out_value *= 16;
                *out_value += str[pos] - '0';
            }else if( (str[pos] >= 'A')&&(str[pos] <= 'F') ){
                *out_value *= 16;
                *out_value += str[pos] - 'A' + 0xA;
            }else if( (str[pos] >= 'a')&&(str[pos] <= 'f') ){
                *out_value *= 16;
                *out_value += str[pos] - 'a' + 0xA;
            }else{
                break;
            }
            pos++;
        };
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int parse_immediate_value(char* str, int* out_value){
    int pos = 0;
    *out_value = 0;
    while( ' ' == str[pos] ){ // 先頭の空白は無視
        pos++;
    }
    if( ('#' == str[pos++]) && ('0' == str[pos++]) && ('x' == str[pos++]) ){ 
        while(1){
            if( (str[pos] >= '0')&&(str[pos] <= '9') ){
                *out_value *= 16;
                *out_value += str[pos] - '0';
            }else if( (str[pos] >= 'A')&&(str[pos] <= 'F') ){
                *out_value *= 16;
                *out_value += str[pos] - 'A' + 0xA;
            }else if( (str[pos] >= 'a')&&(str[pos] <= 'f') ){
                *out_value *= 16;
                *out_value += str[pos] - 'a' + 0xA;
            }else{
                break;
            }
            pos++;
        };
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int is_register(char* str){
    int pos = 0;
    while( ' ' == str[pos] ){ // 先頭の空白は無視
        pos++;
    }

    if( 'r' == str[pos] ){
        return 1;
    }else{
        return 0;
    }
}

int skip_sbracket(char* str){
    int pos = 0;
    while( ' ' == str[pos] ){ // 先頭の空白は無視
        pos++;
    }
    if( ('[' == str[pos])||(']' == str[pos]) ){
        pos++;
        return pos;
    }else{
        return PARSE_FAIL;
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
    }else if( (':' == str[pos])||('.' == str[pos])){
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
    if( 0 == strncmp("mov", OpCode.str, OpCode.len) ){
        int Rd; // Destination Register
        read_len = parse_register(input, &Rd);
        input += read_len;
        
        read_len = skip_comma(input);
        input += read_len;

        int immediate_op;
        int operand2 = 0;
        if(1 == is_register(input)){
            immediate_op = 0;
            int Rm; // 2nd operand register
            read_len = parse_register(input, &Rm);
            operand2 |= Rm;
        }else{
            immediate_op = 1;
            int immediate_value; // unsigned 8bit immediate value ※まだローテートには対応してません
            read_len = parse_immediate_value(input, &immediate_value);
            operand2 |= immediate_value;
        }
        int word = 0xE1A00000;
        word |= Rd<<12; 
        word |= immediate_op<<25;
        word |= operand2;  
        return word;
    }else if( 0 == strncmp(".", OpCode.str, OpCode.len)){
        struct Substring substr_raw; 
        read_len = parse_one(input, &substr_raw); // raw
        assert( 0 == strncmp("raw",substr_raw.str, substr_raw.len) );
        input += read_len;

        int raw_value;
        read_len = parse_raw_value(input,&raw_value); 
        return raw_value;
    }else  if( 0 == strncmp("ldr", OpCode.str, OpCode.len)){
        int Rd; // Destination Register
        read_len = parse_register(input, &Rd);
        input += read_len;
        
        read_len = skip_comma(input);
        input += read_len;
        
        assert(1 == is_sbracket(input));
        read_len = skip_sbracket(input);
        input += read_len;
        
        assert(1 == is_register(input));
        int Rn; // Base Register
        read_len = parse_register(input, &Rn);
        input += read_len;
        
        read_len = skip_comma(input);
        input += read_len;

        int immediate_value;
        read_len = parse_immediate_value(input, &immediate_value);
        input += read_len;
        
        int word = 0xE5900000;
        word |= Rd<<12;
        word |= Rn<<16;
        word |= immediate_value;

        //word = 0xE59F0038; // 1110 01 0 1 1001 1111 0000 000000111000   ldr, [r15, #0x38]
        return word;
    }else{
        return 0;
    }
}

static void test_asm_mov(){
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
static void test_is_register(){
    char* input1 = "mov"; // is not register
    int expect1 = 0;

    char* input2 = " r2"; // is register
    int expect2 = 1;

    int actual1 = is_register(input1);
    int actual2 = is_register(input2);

    assert(expect1 == actual1);
    assert(expect2 == actual2);
}
static void test_parse_immediate_value(){
    char* input = " #0x68 ";
    int expect = 0x68;

    int actual;
    int read_len = parse_immediate_value(input, &actual);

    assert(expect == actual);
}
static void test_parse_immediate_value2(){
    char* input = " #0xA8 ";
    int expect = 0xA8;

    int actual;
    int read_len = parse_immediate_value(input, &actual);
    
    assert(expect == actual);
}
static void test_asm_mov_immediate_value(){
    char* input = "mov r1, #0x68";
    int expect = 0xE3A01068; // 1110 00 1 1101 0 0000 0001 0000 01101000
     
    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_parse_raw_value(){
    char* input = ".raw 0x12345678";
    int expect = 0x12345678;
    
    struct Substring actual_sub1; 
    int read_len = parse_one(input, &actual_sub1); // .
    input += read_len;

    struct Substring actual_sub2; 
    read_len = parse_one(input, &actual_sub2); // raw
    input += read_len;

    int actual;
    read_len = parse_raw_value(input,&actual); //
    
    assert(expect == actual);
}
static void test_asm_raw(){
    char* input = ".raw 0x12345678";
    int expect = 0x12345678; 
     
    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_asm_ldr(){
    char* input = "ldr r0,[r15, #0x38]";
    int expect = 0xE59F0038;

    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_is_sbracket(){
    char* input1 = " ["; 
    int expect1 = 1;

    char* input2 = "r2"; 
    int expect2 = 0;

    int actual1 = is_sbracket(input1);
    int actual2 = is_sbracket(input2);

    assert(expect1 == actual1);
    assert(expect2 == actual2);
}

static void unit_tests(){
    test_asm_mov();
    test_parse_one();
    test_parse_one_indent();
    test_parse_one_label();
    test_parse_one_error();
    test_parse_one_nothing();
    test_parse_register();
    test_parse_register2();
    test_parse_register_and_skip_comma();
    test_is_register();
    test_parse_immediate_value();
    test_parse_immediate_value2();
    test_asm_mov();
    test_asm_mov_immediate_value();
    test_parse_raw_value();
    test_asm_raw();
    test_asm_ldr();
    test_is_sbracket();
}

int main(){
    unit_tests();

    char* input = "mov r1, r2\nmov r3, r4\nmov r5, r6\n"; // expect: 0xE1A01002 0xE1A03004 0xE1A05006
    cl_getline_set_src(input);
    char* buf;
    while( -1 != cl_getline(&buf) ){
        int oneword = asm_one(buf);
        emit_word(&g_emitter, oneword);
    }
    hex_dump(&g_emitter);
    return 0;
}

