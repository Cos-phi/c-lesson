#include "clesson.h"

int substreq(char* s1, struct Substring s2){
    if( 0 == strncmp(s1, s2.str, s2.len) ){
        return 1;
    }else{
        return 0;
    }
}

int streq(char* s1, char* s2){
    if( 0 == strcmp(s1,s2) ){
        return 1;
    }else{
        return 0;
    }
}

int skip_whitespace(char* str){
/*
    文字列冒頭のスペースを読み飛ばして、読み飛ばした数をreturnします。
    e.g. "  abcd.." -> 2
*/
    int pos = 0;
    while( ' ' == str[pos] ){
        pos++;
    }
    return pos;
}

int skip_comma(char* str){
    int pos = skip_whitespace(str);
    if( ',' == str[pos] ){
        pos++;
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int skip_sbracket(char* str){
    int pos = skip_whitespace(str);
    if( ('[' == str[pos])||(']' == str[pos]) ){
        pos++;
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int is_register(char* str){
    int pos = skip_whitespace(str);
    if( 'r' == str[pos] ){
        return 1;
    }else{
        return 0;
    }
}
int is_sbracket(char* str){
    int pos = skip_whitespace(str);
    if( ('[' == str[pos])||(']' == str[pos]) ){
        return 1;
    }else{
        return 0;
    }
}

int parse_raw_value(char* str, int* out_value){ 
/*
    16進数を表す文字列をパースして、数値を返します。読んだ文字数をreturnします。
    e.g. "0x10" -> 16
*/
    *out_value = 0;
    int pos = skip_whitespace(str);
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
/*
    即値を表す文字列をパースして、数値を返します。読んだ文字数をreturnします。
    e.g. " #0x1A" -> 26
*/
    *out_value = 0;
    int pos = skip_whitespace(str);
    if( '#' != str[pos++] ){ 
        return PARSE_FAIL;
    }
    int value_sign;
    if( '-' == str[pos] ){ //即値が負の時  e.g. #-0x123
        value_sign = -1;
        pos++;
    }else{ //即値が正の時
        value_sign = 1;
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
        *out_value *= value_sign;
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int parse_register(char* str, int* out_register){ 
/*
    レジスタを表す文字列をパースして、レジスタ番号を返します。読んだ文字数をreturnします。
    e.g. "r4" -> 4
*/
    int pos = skip_whitespace(str);
    if( 'r' == str[pos] ){
        pos++;
        if( '0'==str[pos]||('2'<=str[pos] && '9'>=str[pos]) ){ //e.g. r0, r2 ~ r9
            *out_register = str[pos]-'0';
            pos++;
        }else if( '1' == str[pos] ){ //e.g. r10 ~ r15
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
/*
    文字列をパースして、文字の単語・コロン・ドット・なにもなしのどれかをSubstringで返します。読んだ文字数をreturnします。
    e.g. "    "      -> "    ",   return 4
         " abc123: " -> "abc123", return 7
         ":"         -> ":",      return 1
*/
    int pos = skip_whitespace(str);
    if( '\0' == str[pos] ){ // 空白のまま終わった場合（なにもなし）
        out_subs->str = str;
        out_subs->len = pos;
        return pos;
    }

    if( ('A'<=str[pos] && 'z'>=str[pos]) || '_'==str[pos] ){ //e.g. "_abc123"
        out_subs->str = &str[pos];
        int start_pos = pos++;
        while( ('0'<=str[pos] && '9'>=str[pos]) || ('A'<=str[pos] && 'z'>=str[pos]) || '_'==str[pos] ){
            pos++;
        } 
        if( (' ' == str[pos]) || (':' == str[pos]) || (',' == str[pos]) || ('\0' == str[pos]) ){
            out_subs->len = (pos - start_pos);
            return pos;
        }else{
            return PARSE_FAIL;
        }
    }else if( (':' == str[pos])||('.' == str[pos])){ //e.g. ":"
        out_subs->str = &str[pos];
        pos++;
        out_subs->len = pos;
        return pos;
    }else{
        return PARSE_FAIL;
    }
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
    assert(substreq(expect_str, actual_sub));
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
    assert(substreq(expect_str, actual_sub));
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

    assert(substreq(expect_str1, actual_sub1));
    assert(substreq(expect_str2, actual_sub2));
}
static void test_parse_one_word(){
    char* input = " word word2";
    char* expect_str1 = "word";
    int expect_len1 = 4;
    char* expect_str2 = "word2";
    int expect_len2 = 5;
    
    struct Substring actual_sub1; 
    struct Substring actual_sub2; 
    input += parse_one(input, &actual_sub1);
    parse_one(input, &actual_sub2);

    assert(substreq(expect_str1, actual_sub1));
    assert(expect_len1 == actual_sub1.len);
    assert(substreq(expect_str2, actual_sub2));
    assert(expect_len2 == actual_sub2.len);
}
static void test_parse_one_word_and_comma(){
    char* input = " word, word2";
    char* expect_str1 = "word";
    int expect_len1 = 4;
    char* expect_str2 = "word2";
    int expect_len2 = 5;
    
    struct Substring actual_sub1; 
    struct Substring actual_sub2; 
    input += parse_one(input, &actual_sub1);
    parse_one(input, &actual_sub2);

    assert(substreq(expect_str1, actual_sub1));
    assert(expect_len1 == actual_sub1.len);
    assert(substreq(expect_str2, actual_sub2));
    assert(expect_len2 == actual_sub2.len);
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
    assert(substreq(expect_str, actual_sub));
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
static void test_parse_immediate_value3(){
    char* input = " #-0xA8 ";
    int expect = -0xA8;

    int actual;
    int read_len = parse_immediate_value(input, &actual);
    
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


void parser_unittests(){
    test_parse_one();
    test_parse_one_indent();
    test_parse_one_label();
    test_parse_one_error();
    test_parse_one_word();
    test_parse_one_word_and_comma();
    test_parse_one_nothing();
    test_parse_register();
    test_parse_register2();
    test_parse_register_and_skip_comma();
    test_is_register();
    test_parse_immediate_value();
    test_parse_immediate_value2();
    test_parse_immediate_value3();

    test_parse_raw_value();
    test_is_sbracket();


}