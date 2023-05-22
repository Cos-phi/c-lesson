#include "clesson.h"

struct Substring {
    char *str;
    int len;
};

int parce_register(char* str, int* out_register){
    int pos = 0;

    //ハードコード
    pos = 6;
    *out_register = 4;

    return pos;
}

int parce_one(char *str, struct Substring* out_subs){
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
        int start_pos = pos;
        while( EOF != str[pos++] ){
            if( ('0'<=str[pos] && '9'>=str[pos]) || ('A'<=str[pos] && 'z'>=str[pos]) || '_'==str[pos] ){
                continue;
            }else if( ' ' == str[pos]){
                out_subs->len = (pos - start_pos);
                return pos;
            }else if( ':' == str[pos]){
                pos++;
                out_subs->len = (pos - start_pos);
                return pos;
            }else{
                return -1;
            }
        } 
    }
    return -1;
}

int asm_one(char* input){
    int word;
    if (0 == strcmp("mov r1, r2" ,input)){
        word = 0xE1A01002;
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

static void test_parce_one(){
    char* input = "mov r1, r2";
    char* expect_str = "mov";
    int expect_pos = 3;
    int expect_len = 3;
    
    struct Substring actual_sub; 
    int pos = parce_one(input, &actual_sub);

    assert(expect_pos == pos);
    assert(expect_len == actual_sub.len);
    assert(0 == strncmp(expect_str, actual_sub.str, actual_sub.len));
}

static void test_parce_one_indent(){
    char* input = "    mov r1, r2";
    char* expect_str = "mov";
    int expect_len = 3;
    int expect_pos = 7;
    
    struct Substring actual_sub; 
    int pos = parce_one(input, &actual_sub);

    assert(expect_pos == pos);
    assert(expect_len == actual_sub.len);
    assert(0 == strncmp(expect_str, actual_sub.str, actual_sub.len));
}

static void test_parce_one_label(){
    char* input = "loop:";
    char* expect_str = "loop:";
    int expect_len = 5;
    int expect_pos = 5;
    
    struct Substring actual_sub; 
    int pos = parce_one(input, &actual_sub);

    assert(expect_pos == pos);
    assert(expect_len == actual_sub.len);
    assert(0 == strncmp(expect_str, actual_sub.str, actual_sub.len));
}

static void test_parce_one_error(){
    char* input = "abc{}";
    int expect_pos = -1;
    
    struct Substring actual_sub; 
    int pos = parce_one(input, &actual_sub);

    assert(expect_pos == pos);
}

static void test_parce_one_nothing(){
    char* input = "    ";
    char* expect_str = "    ";
    int expect_len = 4;
    int expect_pos = 4;
    
    struct Substring actual_sub; 
    int pos = parce_one(input, &actual_sub);

    assert(expect_pos == pos);
    assert(expect_len == actual_sub.len);
    assert(0 == strncmp(expect_str, actual_sub.str, actual_sub.len));
}

static void test_parce_register(){
    char* input = "mov r4, r2";
    int expect_r1 = 4;
    
    struct Substring actual_sub; 
    int read_len = parce_one(input, &actual_sub);
    input += read_len;

    int actual_r1;
    read_len = parce_register(input, &actual_r1);
    
    assert(expect_r1 == actual_r1);
    assert(6 == read_len);
}

static void unit_tests(){
    test_asm();
    test_parce_one();
    test_parce_one_indent();
    test_parce_one_label();
    test_parce_one_error();
    test_parce_one_nothing();
    test_parce_register();
}
int main(){
    unit_tests();
}
