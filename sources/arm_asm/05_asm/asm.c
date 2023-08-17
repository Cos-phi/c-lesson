#include "clesson.h"
#define EMITTER_ARRAY_SIZE 250
#define UNRESOLVED_ARRAY_SIZE 128

/*
    g_emitter: アセンブルされた結果を格納する配列です。
    emitter.pos の 0, 1, 2, 3,.. が、実際のバイナリの位置0x0, 0x4, 0x8, 0xC,.. に対応します。
*/
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

void write_emitter_to_file(struct Emitter* emitter, FILE* fp){ //fp mode: wb
    fwrite(emitter->words,sizeof(int),emitter->pos,fp);
}


/*
    unresolved_items: アドレスの解決が必要な物を集めるリストです。
    アドレス（位置）は、emitter.posで表されます。
*/
struct Unresolved_item {
    int label_symbol;
    int emitter_pos;
    int mnemonic_symbol;
};
struct Unresolved_item unresolved_items[UNRESOLVED_ARRAY_SIZE];
int unresolved_items_num = 0;

void clear_unresolved_items(){
    unresolved_items_num = 0;
}

void put_unresolved_item(struct Unresolved_item input_item){
    unresolved_items[unresolved_items_num] = input_item;
    unresolved_items_num++;
}

int get_unresolved_item(struct Unresolved_item* out_item){
    if( 0 == unresolved_items_num ){ //リストが空のとき0を返す
        return 0;
    }else{ //リストが空でないとき1を返す
        unresolved_items_num--;
        *out_item = unresolved_items[unresolved_items_num];
        return 1;
    }
}


/*
    ニモニックをシンボル化した数値を、グローバル変数に持っておきます。
*/
int mov_symbol;
int ldr_symbol;
int str_symbol;
int raw_symbol;
int b_symbol;

void init_mnemonic_sybols(){
    mov_symbol = to_mnemonic_symbol("mov",3);
    ldr_symbol = to_mnemonic_symbol("ldr",3);
    str_symbol = to_mnemonic_symbol("str",3);
    raw_symbol = to_mnemonic_symbol(".",1);
    b_symbol = to_mnemonic_symbol("b",1);
}


/*
    以下、アセンブルを行う関数です。
*/
int asm_one(char* input,int emitter_pos){
/*
    一行の文字列を32bitのバイナリにアセンブルして、intとしてreturnします。
    e.g. "mov r1, r2"           -> 0xE1A01002
         ".raw 0x12345678"      -> 0x12345678
         "ldr r1,[r15, #-0x30]" -> 0xE51F1030
*/
    struct Substring opcode; 
    int read_len = parse_one(input, &opcode);
    input += read_len;

    int mnemonic_sybol = substr_to_mnemonic_symbol(opcode);
    if( mnemonic_sybol == mov_symbol ){
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
    }else if( mnemonic_sybol == raw_symbol ){ 
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
    }else  if( mnemonic_sybol == ldr_symbol || mnemonic_sybol == str_symbol ){ 
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
        if( mnemonic_sybol == ldr_symbol ) { 
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
    }else if( mnemonic_sybol == b_symbol ){    
    /*
        bのケース
        e.g. "b label" 
        ラベルの部分には000000を入れておき、解決が必要なものを集めるリスト（unresolved_items）に登録する。
    */    
        int word = 0xEA000000;

        struct Substring label_str;
        parse_one(input, &label_str);

        struct Unresolved_item unresolved_item;
        unresolved_item.label_symbol = substr_to_label_symbol(label_str);
        unresolved_item.emitter_pos = emitter_pos; 
        unresolved_item.mnemonic_symbol = mnemonic_sybol;
        put_unresolved_item(unresolved_item);
        return word;
    }else{
        return 0;
    }
}

void asm_main(struct Emitter* emitter){
/*
    cl_getlineにセットされた内容をアセンブルします。
    アセンブルした結果は、受け取ったEmitterに格納されます。
*/
    char* buff_line;
    while( -1 != cl_getline(&buff_line) ){
        struct Substring stem; 
        struct Substring suffix; 
        int read_len = parse_one(buff_line, &stem);
        parse_one((buff_line + read_len), &suffix);
        if(substreq(":",suffix)){ // ラベルの場合
            int label_symbol = substr_to_label_symbol(stem);
            int label_address = emitter->pos; //ここで"address"は、emitter内のwordの順番を指すものとします。
            address_put(label_symbol,label_address); 
        }else{ // ニーモニックの場合
            int oneword = asm_one(buff_line,emitter->pos); 
            emit_word(emitter, oneword);
        }
    }
    struct Unresolved_item buff_item;
    while( 0 != get_unresolved_item(&buff_item)){
        int label_address = address_get(buff_item.label_symbol);
        int address_offset = label_address - buff_item.emitter_pos - 2 ;// r15(PC)は2つ先を指しているので、2を引きます。
        int unresolved_word = emitter->words[buff_item.emitter_pos];
        int resolved_word = (unresolved_word&0xFF000000) | (address_offset&0x00FFFFFF) ;
        emitter->words[buff_item.emitter_pos] =  resolved_word;
    }
}

void asm_file(char* input_filename, char* output_filename){
/*    
    .ksファイルのファイル名と、出力ファイル名を受け取って
    アセンブルします。
*/
    FILE* input_fp = fopen(input_filename,"r");
    assert(NULL != input_fp);
    cl_getline_set_file(input_fp);
    init_emitter(&g_emitter);
    asm_main(&g_emitter);
    fclose(input_fp);  
    FILE* output_fp = fopen(output_filename,"wb");
    assert(NULL != output_fp);
    write_emitter_to_file(&g_emitter, output_fp);
    fclose(output_fp);
}


/*
    ユニットテスト
*/
static void test_asm_mov(){
    char* input = "mov r1, r2";
    int expect = 0xE1A01002; // 1110 00 0 1101 0 0000 0001 00000000 0002
     
    int actual = asm_one(input,0);

    assert(expect == actual);
}
static void test_asm_mov_immediate_value(){
    char* input = "mov r1, #0x68";
    int expect = 0xE3A01068; // 1110 00 1 1101 0 0000 0001 0000 01101000
     
    int actual = asm_one(input,0);

    assert(expect == actual);
}
static void test_asm_raw(){
    char* input = ".raw 0x12345678";
    int expect = 0x12345678; 
     
    int actual = asm_one(input,0);

    assert(expect == actual);
}
static void test_asm_ldr(){
    char* input = "ldr r1,[r15, #0x30]";
    int expect = 0xE59F1030;

    int actual = asm_one(input,0);

    assert(expect == actual);
}
static void test_asm_ldr2(){
    char* input = "ldr r1,[r15, #-0x30]";
    int expect = 0xE51F1030; // 1110 01 0 1 0001 1111 0000 000000110000 

    int actual = asm_one(input,0);

    assert(expect == actual);
}
static void test_asm_ldr3(){
    char* input = "ldr r1,[r15]";
    int expect = 0xE59F1000; // 1110 01 1 0 1001 1111 0001 00000000 0000

    int actual = asm_one(input,0);

    assert(expect == actual);
}
static void test_asm_ldr4(){
    char* input = "ldr r0,[r15,#0x38]";
    int expect = 0xE59F0038; 

    int actual = asm_one(input,0);

    assert(expect == actual);
}
static void test_asm_str(){
    char* input = "str r0,[r1]";
    int expect = 0xE5810000; // 1110 01 1 0 1000 0001 0000 00000000 0000

    int actual = asm_one(input,0);

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
        int oneword = asm_one(buf,g_emitter.pos);
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
static void test_asm_b_firstpass(){
    char* input = "b label";
    int expect = 0xEA000000;

    init_emitter(&g_emitter);
    init_label_tree();
    struct Unresolved_item unresolved_item;
    assert(0 == get_unresolved_item(&unresolved_item));
    int expect_emitter_pos = g_emitter.pos;
    int actual = asm_one(input,g_emitter.pos);

    assert(expect == actual);
    assert(1 == get_unresolved_item(&unresolved_item));
    assert(to_label_symbol("label",5) == unresolved_item.label_symbol);
    assert(to_mnemonic_symbol("b",1) == unresolved_item.mnemonic_symbol);
    assert(expect_emitter_pos == unresolved_item.emitter_pos);
}
static void test_asm_file_b(){
/*
    input:次のような内容のアスキー形式のファイルを読み込み、
        ldr r0,[r15,#0x38]
        mov r1,#0x68
        str r1,[r0]
        mov r1,#0x65
        str r1,[r0]
        mov r1,#0x6c
        str r1,[r0]
        mov r1,#0x6c
        str r1,[r0]
        mov r1,#0x6f
        str r1,[r0]
        mov r2,#0x0D
        str r2,[r0]
        mov r2,#0x0A
        str r2,[r0]
    loop:
        b loop
    .raw 0x101f1000

    epect:バイナリ実行ファイルを書き出す       
*/
    char* input_file = "test/test_input/hello_arm.ks";
    int expect_words[4] = {0xE59F1004,0xE3A00068,0xE5810000,0x101F1000};

    char* output_file = "hello_arm_ks.bin";
    asm_file(input_file,output_file);

}
static void asm_unittests(){
    test_asm_mov();
    test_asm_mov();
    test_asm_mov_immediate_value();
    test_asm_raw();
    test_asm_ldr();
    test_asm_ldr2();
    test_asm_ldr3();
    test_asm_ldr4();
    test_asm_str();
    test_cl_getline_file();
    test_asm_ks();
    test_asm_file();
    test_asm_file_init_emitter();
    test_asm_b_firstpass();
    test_asm_file_b();
}

static void unittests(){
    cl_getline_unittests();
    cl_binarytree_unittests();
    dict_unittests();
    parser_unittests();

    init_mnemonic_sybols();
    asm_unittests();
}

int main(int argc, char* argv[]){
    init_mnemonic_sybols();
    if(3 == argc){
        asm_file(argv[1],argv[2]);
    }
    unittests();
    return 0;
}

