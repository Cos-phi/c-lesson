#include "clesson.h"
#define PARSE_FAIL -1
#define EMITTER_ARRAY_SIZE 250

struct Substring {
    char *str;
    int len;
};

int substr_to_mnemonic_symbol(struct Substring substr){
    return to_mnemonic_symbol(substr.str, substr.len);
}

int substr_to_label_symbol(struct Substring substr){
    return to_label_symbol(substr.str, substr.len); 
}

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

struct Emitter {
    int* words;
    int pos;
};

int array[EMITTER_ARRAY_SIZE]; 
struct Emitter g_emitter = {array,0};

void init_emitter(struct Emitter* emitter){
    emitter->pos = 0;
}

void emit_word(struct Emitter* emitter, int oneword){
    emitter->words[emitter->pos] = oneword;
    emitter->pos++;
}

void hex_dump(struct Emitter* emitter){
    for (int i = 0; i < emitter->pos; i++){
        printf("0x%X\n",emitter->words[i]);
    }
}

void write_emitter_to_file(struct Emitter* emitter, FILE* fp){
/*
emitterと、モードwbでオープンされたファイルポインタを受け取って、emitterの中身をファイルに書き込みます。
*/    
    fwrite(emitter->words,sizeof(int),emitter->pos,fp);
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
        if( (' ' == str[pos]) || (':' == str[pos]) ){
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

int asm_one(char* input){
/*
    一行の文字列を32bitのバイナリにアセンブルして、intとしてreturnします。
    e.g. "mov r1, r2"           -> 0xE1A01002
         ".raw 0x12345678"      -> 0x12345678
         "ldr r1,[r15, #-0x30]" -> 0xE51F1030
*/
    struct Substring opcode; 
    int read_len = parse_one(input, &opcode);
    input += read_len;
    if( substreq("mov", opcode) ){ 
    /*
        movのケース
        e.g. "mov r1, r2"   -> Rdにr1が、Rmに2が入る
             "mov r1, #123" -> Rdにr1が、immediate_valueに123が入る
    */
        int Rd; 
        read_len = parse_register(input, &Rd);
        input += read_len;
        
        read_len = skip_comma(input); 
        input += read_len;

        int immediate_op;
        int operand2 = 0;
        if(1 == is_register(input)){ //e.g. mov r1, r2
            immediate_op = 0;
            int Rm; 
            read_len = parse_register(input, &Rm);
            operand2 |= Rm;
        }else{ //e.g. mov r1, #123
            immediate_op = 1;
            int immediate_value; 
            read_len = parse_immediate_value(input, &immediate_value);
            assert( 0 < immediate_value );
            operand2 |= immediate_value;
        }
        int word = 0xE1A00000;
        word |= Rd<<12; 
        word |= immediate_op<<25;
        word |= operand2;  
        return word;
    }else if( substreq(".", opcode)){ 
    /*
        疑似命令.rawのケース
        e.g. ".raw 0x123456" 
        引数の数値0x123456が、そのままraw_valueに入る
    */
        struct Substring substr_raw; 
        read_len = parse_one(input, &substr_raw);
        assert( substreq("raw",substr_raw) );
        input += read_len;

        int raw_value;
        read_len = parse_raw_value(input,&raw_value); 
        return raw_value;
    }else  if( substreq("ldr", opcode) || substreq("str", opcode) ){ 
    /*
        ldr または str のケース
        e.g. "ldr r1, [r2]"       -> Rdにr1が、Rnにr2が入る。
             "ldr r1, [r2,#0x12]" -> Rdにr1、Rnにr2、immediate_valueに0x12が入る。
        opcodeにldrかstrが入っている。
    */
        int Rd; 
        read_len = parse_register(input, &Rd); 
        input += read_len;
        
        read_len = skip_comma(input); 
        input += read_len;
        
        assert(1 == is_sbracket(input)); 
        read_len = skip_sbracket(input); 
        input += read_len;
        
        assert(1 == is_register(input)); 
        int Rn; 
        read_len = parse_register(input, &Rn); 
        input += read_len;
        
        int word;
        if( substreq("ldr", opcode) ) { 
            word = 0xE5900000 ; 
        }else{ // "str"
            word = 0xE5800000 ; 
        }

        if( 1 == is_sbracket(input) ){ //e.g. ldr r1, [r2]
            word |= Rn<<16;
            word |= Rd<<12; 
        }else{ //e.g. ldr r1, [r2,#0x12]
            read_len = skip_comma(input);
            input += read_len;

            int immediate_value;
            read_len = parse_immediate_value(input, &immediate_value);
            input += read_len;

            if( 0 > immediate_value ){ 
                word &= 0xFF7FFFFF;
            }
            word |= Rn<<16;
            word |= Rd<<12;
            word |= abs(immediate_value);
        }
        return word;
    }else{
        return 0;
    }
}

void asm_file(char* input_filename, char* output_filename){
/*    
    .ksファイルのファイル名と、出力ファイル名を受け取って
    アセンブルする。
*/
    FILE* input_fp = fopen(input_filename,"r");
    assert(NULL != input_fp);
    cl_getline_set_file(input_fp);
    char* buf;
    init_emitter(&g_emitter);
    while( -1 != cl_getline(&buf) ){
        int oneword = asm_one(buf);
        emit_word(&g_emitter, oneword);
    }
    fclose(input_fp);  
    FILE* output_fp = fopen(output_filename,"wb");
    assert(NULL != output_fp);
    write_emitter_to_file(&g_emitter, output_fp);
    fclose(output_fp);
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
static void test_asm_ldr(){
    char* input = "ldr r1,[r15, #0x30]";
    int expect = 0xE59F1030;

    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_asm_ldr2(){
    char* input = "ldr r1,[r15, #-0x30]";
    int expect = 0xE51F1030; // 1110 01 0 1 0001 1111 0000 000000110000 

    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_asm_ldr3(){
    char* input = "ldr r1,[r15]";
    int expect = 0xE59F1000; // 1110 01 1 0 1001 1111 0001 00000000 0000

    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_asm_str(){
    char* input = "str r0,[r1]";
    int expect = 0xE5810000; // 1110 01 1 0 1000 0001 0000 00000000 0000

    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_cl_getline_file(){
    char* input_file = "test/test_input/nanika_mojiwo_hyouji.ks";
    char* expect_str1 = "ldr r1, [r15, #0x04]";
    char* expect_str2 = "mov r0, #0x68";
    char* expect_str3 = "str r0, [r1]";
    char* expect_str4 = ".raw 0x101f1000";
    char** expect_lines[4] = {&expect_str1,&expect_str2,&expect_str3,&expect_str4};

    FILE *file = fopen(input_file,"r");
    cl_getline_set_file(file);

    char* buf;
    int i = 0;
    while( -1 != cl_getline(&buf) ){
        assert(streq(buf,*expect_lines[i++]));
    }
    fclose(file);  
}
static void test_asm_ks(){
/*
    input:次のような内容のアスキー形式のファイルを読み込み、
        ldr r1, [r15, #0x04]
        mov r0, #0x68
        str r0, [r1]
        .raw 0x101f1000

    epect:以下のワードからなるバイナリ実行ファイルを書き出す       
        0xE59F1004 0xE3A00068 0xE5810000 0x101F1000
*/
    char* input_file = "test/test_input/nanika_mojiwo_hyouji.ks";
    int expect_words[4] = {0xE59F1004,0xE3A00068,0xE5810000,0x101F1000};

    FILE* input_fp = fopen(input_file,"r");
    assert(NULL != input_fp);
    cl_getline_set_file(input_fp);
    char* buf;
    init_emitter(&g_emitter);
    while( -1 != cl_getline(&buf) ){
        int oneword = asm_one(buf);
        emit_word(&g_emitter, oneword);
    }
    fclose(input_fp);  
    char* output_file = "nanika_mojiwo_hyouji.bin";
    FILE* output_fp = fopen(output_file,"wb");
    write_emitter_to_file(&g_emitter, output_fp);
    fclose(output_fp);

    FILE* actual_fp = fopen(output_file,"rb");
    int actual_words[4];
    fread(actual_words,sizeof(int),4,actual_fp);
    fclose(actual_fp);
    for (int i = 0; i < 4; i++){
        assert( expect_words[i] == actual_words[i] );
    }
    remove(output_file);
}
static void test_asm_file(){
/*
    input:次のような内容のアスキー形式のファイルを読み込み、
        ldr r1, [r15, #0x04]
        mov r0, #0x68
        str r0, [r1]
        .raw 0x101f1000

    epect:以下のワードからなるバイナリ実行ファイルを書き出す       
        0xE59F1004 0xE3A00068 0xE5810000 0x101F1000
*/
    char* input_file = "test/test_input/nanika_mojiwo_hyouji.ks";
    int expect_words[4] = {0xE59F1004,0xE3A00068,0xE5810000,0x101F1000};

    char* output_file = "nanika_mojiwo_hyouji.bin";
    asm_file(input_file,output_file);

    FILE* actual_fp = fopen(output_file,"rb");
    int actual_words[4];
    fread(actual_words,sizeof(int),4,actual_fp);
    fclose(actual_fp);
    for (int i = 0; i < 4; i++){
        assert( expect_words[i] == actual_words[i] );
    }
    remove(output_file);
}
static void test_asm_file_init_emitter(){
/*
    複数回g_emitterを読み書きをした後、ファイルを正しく出力できているか確認する
*/    
    char* input_file = "test/test_input/nanika_mojiwo_hyouji.ks";
    int expect_words[4] = {0xE59F1004,0xE3A00068,0xE5810000,0x101F1000};

    init_emitter(&g_emitter);
    emit_word(&g_emitter, 0xABCD1234);
    emit_word(&g_emitter, 0x12345678);
    emit_word(&g_emitter, 0xABAB1212);

    char* output_file = "nanika_mojiwo_hyouji_tmp.bin";
    asm_file(input_file,output_file);

    FILE* actual_fp = fopen(output_file,"rb");
    int actual_words[4];
    fread(actual_words,sizeof(int),4,actual_fp);
    fclose(actual_fp);
    for (int i = 0; i < 4; i++){
        assert( expect_words[i] == actual_words[i] );
    }
    remove(output_file);
}
static void asm_unittests(){
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
    test_parse_immediate_value3();
    test_asm_mov();
    test_asm_mov_immediate_value();
    test_parse_raw_value();
    test_asm_raw();
    test_is_sbracket();
    test_asm_ldr();
    test_asm_ldr2();
    test_asm_ldr3();
    test_asm_str();
    test_cl_getline_file();
    test_asm_ks();
    test_asm_file();
    test_asm_file_init_emitter();
}

int main(int argc, char* argv[]){
    asm_unittests();
    cl_getline_unittests();
    cl_binarytree_unittests();
    if(3 == argc){
        asm_file(argv[1],argv[2]);
    }
    return 0;
}

