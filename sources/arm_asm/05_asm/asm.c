#include "clesson.h"
#define EMITTER_ARRAY_SIZE 250
#define UNRESOLVED_ARRAY_SIZE 128
#define RAWSTR_BUFFSIZE 64

/*
    g_emitter: アセンブルされた結果を格納する配列です。
    emitter.pos の 0, 1, 2, 3,.. が、実際のバイナリの位置0x0, 0x4, 0x8, 0xC,.. に対応します。
*/
struct Emitter {
    int* words;
    int pos;
};
static int array[EMITTER_ARRAY_SIZE]; 
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
        printf("%d 0x%X\n",i,emitter->words[i]);
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
int ldrb_symbol;
int str_symbol;
int dot_symbol;
int b_symbol;
int bne_symbol;
int cmp_symbol;
int add_symbol;

void init_mnemonic_symbols(){
    mov_symbol = to_mnemonic_symbol("mov",3);
    ldr_symbol = to_mnemonic_symbol("ldr",3);
    ldrb_symbol = to_mnemonic_symbol("ldrb",4);
    str_symbol = to_mnemonic_symbol("str",3);
    dot_symbol = to_mnemonic_symbol(".",1);
    b_symbol = to_mnemonic_symbol("b",1);
    bne_symbol = to_mnemonic_symbol("bne",3);
    cmp_symbol = to_mnemonic_symbol("cmp",3);
    add_symbol = to_mnemonic_symbol("add",3);
}


/*
    以下、アセンブルを行う関数です。
*/
int asm_one(char* input){
/*
    入力された文字列のみからアセンブルした結果が定まる文字列について
    32bitのバイナリにアセンブルして、intとしてreturnします。
    e.g. "mov r1, r2"           -> 0xE1A01002
         ".raw 0x12345678"      -> 0x12345678
         "ldr r1,[r15, #-0x30]" -> 0xE51F1030
*/
    struct Substring opcode; 
    input += parse_one(input, &opcode);

    int mnemonic_symbol = substr_to_mnemonic_symbol(opcode);
    if( mnemonic_symbol == mov_symbol ){
    /*
        movのケース
        e.g. "mov r1, r2"   -> Rdにr1が、Rmに2が入る
             "mov r1, #123" -> Rdにr1が、immediate_valueに123が入る
    */
        int Rd; 
        input += parse_register(input, &Rd);
        input += skip_comma(input); 

        int immediate_op;
        int operand2 = 0;
        if(1 == is_register(input)){ //e.g. mov r1, r2
            immediate_op = 0;
            int Rm; 
            parse_register(input, &Rm);
            operand2 |= Rm;
        }else{ //e.g. mov r1, #123
            immediate_op = 1;
            int immediate_value; 
            parse_immediate_value(input, &immediate_value);
            assert( 0 < immediate_value );
            operand2 |= immediate_value;
        }
        int word = 0xE1A00000;
        word |= Rd<<12; 
        word |= immediate_op<<25;
        word |= operand2;  
        return word;
    }else  if( mnemonic_symbol == ldr_symbol || mnemonic_symbol == str_symbol || mnemonic_symbol == ldrb_symbol){ 
    /*
        ldr または ldrb または str のケース
        e.g. "ldr r1, [r2]"       -> Rdにr1が、Rnにr2が入る。
             "ldr r1, [r2,#0x12]" -> Rdにr1、Rnにr2、immediate_valueに0x12が入る。
        opcodeにldrかldrbかstrが入っている。
    */
        int Rd; 
        input += parse_register(input, &Rd); 
        input += skip_comma(input); 
        assert(1 == is_sbracket(input)); 
        input += skip_sbracket(input); 
        assert(1 == is_register(input)); 
        int Rn; 
        input += parse_register(input, &Rn); 
        
        int word;
        if( mnemonic_symbol == ldr_symbol ) { 
            word = 0xE5900000; 
        }else if( mnemonic_symbol == str_symbol ) { // "str"
            word = 0xE5800000; 
        }else{ // "ldrb"
            word = 0xE5D00000;
        }

        if( 1 == is_sbracket(input) ){ //e.g. ldr r1, [r2]
            word |= Rn<<16;
            word |= Rd<<12; 
        }else{ //e.g. ldr r1, [r2,#0x12]
            input += skip_comma(input);
            int immediate_value;
            parse_immediate_value(input, &immediate_value);
            if( 0 > immediate_value ){ 
                word &= 0xFF7FFFFF;
            }
            word |= Rn<<16;
            word |= Rd<<12;
            word |= abs(immediate_value);
        }
        return word;
    }else if( mnemonic_symbol == cmp_symbol ){
    /*
        cmpのケース
        e.g. cmp r3,#0 
        e.g. cmp r1,r2
    */
        int word = 0xE1500000;
        int Rn;
        input += parse_register(input, &Rn);
        word |= Rn<<16;
        input += skip_comma(input); 
        if( 1 == is_register(input) ){
            int Rm;
            parse_register(input, &Rm);
            word |= Rm;
        }else{
            word |= 0x02000000;
            int immediate_value;
            parse_immediate_value(input, &immediate_value);
            word |= (immediate_value & 0x000000FF);
        }
        return word;

    }else if( mnemonic_symbol == add_symbol ){
    /*
        addのケース
        e.g. add r1, r1, #5
    */   
        int word = 0xE2800000;
        int Rd;
        int Rn;
        int immediate_value;
        input += parse_register(input, &Rd);
        input += skip_comma(input); 
        input += parse_register(input, &Rn);
        input += skip_comma(input); 
        input += parse_immediate_value(input, &immediate_value);
        word |= Rd<<12;
        word |= Rn<<16;
        word |= immediate_value;
        return word;
    }else{
        return 0;
    }
}

void asm_line(char* input, struct Emitter* emitter){
/*
    1行の文字列を32bitのバイナリにアセンブルして、emitterに追加します。
*/
    struct Substring opcode; 
    int read_len = parse_one(input, &opcode);
    int mnemonic_symbol = substr_to_mnemonic_symbol(opcode);
    
    if( mnemonic_symbol == b_symbol || mnemonic_symbol == bne_symbol ){    
    /*
        b/bneのケース
        e.g. "b label" 
        即値（ラベルが指すアドレス）には000000を入れておき、解決が必要なものを集めるリスト（unresolved_items）に登録する。
    */    
        input += read_len;
        struct Substring label_str;
        parse_one(input, &label_str);

        struct Unresolved_item unresolved_item;
        unresolved_item.label_symbol = substr_to_label_symbol(label_str);
        unresolved_item.emitter_pos = emitter->pos; 
        unresolved_item.mnemonic_symbol = mnemonic_symbol;
        put_unresolved_item(unresolved_item);
        
        int dummyword;
        if( mnemonic_symbol == b_symbol ){
            dummyword = 0xEA000000;
        }else{
            dummyword = 0x1A000000;
        }
        emit_word(emitter, dummyword);
    }else if( mnemonic_symbol == ldr_symbol ){
    /*
        ldrのケース
        ラベルが指すアドレスには000000を入れておき、解決が必要なものを集めるリスト（unresolved_items）に登録する
        ラベルを指している場合以外は asm_oneで処理する
    */  
        char* tmp_input = input + read_len;
        int Rd;
        tmp_input += parse_register(tmp_input, &Rd);
        tmp_input += skip_comma( tmp_input );
        if( 1 == is_equal(tmp_input) ){ // ラベルのケース e.g. ldr r1,=label
            tmp_input += skip_equal(tmp_input);
            struct Substring label_str;
            parse_one(tmp_input, &label_str);

            struct Unresolved_item unresolved_item;
            unresolved_item.label_symbol = substr_to_label_symbol(label_str);
            unresolved_item.emitter_pos = emitter->pos; 
            unresolved_item.mnemonic_symbol = mnemonic_symbol;
            put_unresolved_item(unresolved_item);

            int dummyword = 0xE59F0000;
            dummyword |= Rd<<12;
            emit_word(emitter, dummyword);
        }else{ // ラベル以外のケース
            int oneword = asm_one(input);
            emit_word(emitter, oneword);
        }

    }else if( mnemonic_symbol == dot_symbol ){ 
    /*
        疑似命令.rawのケース
    */
        input += read_len;
        struct Substring pseudo_inst_name; 
        input += parse_one(input, &pseudo_inst_name);
        assert( substreq("raw",pseudo_inst_name) );
        input += skip_whitespace(input);
        int raw_value;
        if( 1 == is_doublequotation(input) ){
        /*
            引数をエスケープ処理した文字列が、4文字ずつ文字列をintにしてemitterに入る
            e.g. ".raw \"Hello world\"" -> 0x6C6C6568 0x6F77206F 0x00646C72 がEmitterに入る
        */
            char* str;
            raw_value = 0;
            parse_string(input,&str);

            int raw_value_words[RAWSTR_BUFFSIZE];
            int raw_value_words_index = 0;
            raw_value_words[raw_value_words_index] = 0;
            for(int i=0; '\0' != str[i];i++){
                if( 0 == i%4 && 0 != i ){
                    raw_value_words_index++;
                    raw_value_words[raw_value_words_index] = 0;
                }
                raw_value_words[raw_value_words_index] += str[i] << 8*i;
            }
            for(int i=0; i <= raw_value_words_index; i++){
                emit_word(emitter,raw_value_words[i]);
            }
        }else{
        /*
            e.g. ".raw 0x123456" 
            引数の数値0x123456が、そのままraw_valueに入りemitterに入る
        */
            parse_raw_value(input,&raw_value); 
            emit_word(emitter, raw_value);
        }
    }else{
    /*
        その他、asm_oneで処理できる（emitterの状態に依存しない）ニーモニックのケース
    */
        int oneword = asm_one(input);
        emit_word(emitter, oneword);
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
            asm_line(buff_line,emitter); 
        }
    }
    struct Unresolved_item unresolved_item;
    while( 0 != get_unresolved_item(&unresolved_item)){
        int label_pos = address_get(unresolved_item.label_symbol);
        int unresolved_word = emitter->words[unresolved_item.emitter_pos];
        if( ldr_symbol == unresolved_item.mnemonic_symbol ){
            int coefficient_pos_to_address = 4;
            int label_address = 0x00010000 + label_pos * coefficient_pos_to_address;
            emit_word(emitter, label_address); 
            int offset = (emitter->pos - 1) - unresolved_item.emitter_pos - 2;// emitter.posは1つ先を、r15(PC)は2つ先を指しているので、1,2を引きます。
            int address_offset = offset * coefficient_pos_to_address;
            int resolved_word = (unresolved_word&0xFFFFF000) | (address_offset&0x00000FFF) ;
            emitter->words[unresolved_item.emitter_pos] =  resolved_word;
        }else{ // e.g. b, bl, bne..
            int offset = label_pos - unresolved_item.emitter_pos - 2;// r15(PC)は2つ先を指しているので、2を引きます。
            int resolved_word = (unresolved_word&0xFFFF0000) | (offset&0x00FFFFFF) ;
            emitter->words[unresolved_item.emitter_pos] =  resolved_word;
        }
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
     
    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_asm_mov_immediate_value(){
    char* input = "mov r1, #0x68";
    int expect = 0xE3A01068; // 1110 00 1 1101 0 0000 0001 0000 01101000
     
    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_asm_raw(){
    char* input = ".raw 0x12345678";
    int expect = 0x12345678; 

    init_emitter(&g_emitter); 
    asm_line(input,&g_emitter);

    assert(expect == g_emitter.words[0]);
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
static void test_asm_ldr4(){
    char* input = "ldr r0,[r15,#0x38]";
    int expect = 0xE59F0038; 

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

    expect:以下のワードからなるバイナリ実行ファイルを書き出す       
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
        asm_line(buf,&g_emitter);
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

    expect:以下のワードからなるバイナリ実行ファイルを書き出す       
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
    asm_line(input,&g_emitter);

    assert(expect == g_emitter.words[0]);
    assert(1 == get_unresolved_item(&unresolved_item));
    assert(to_label_symbol("label",5) == unresolved_item.label_symbol);
    assert(to_mnemonic_symbol("b",1) == unresolved_item.mnemonic_symbol);
    assert(expect_emitter_pos == unresolved_item.emitter_pos);
}
static void test_asm_file_b(){
/*
    動作確認のためファイルに出力するだけの関数です。
    この関数内では、結果の判定を行いません。

    input:  hello_arm.s相当のファイルhello_arm.ksを読み込み、
    expect: hello_arm.sをARMアセンブラでアセンブルしたバイナリと、同等のバイナリを書き出す。
*/
    char* input_file = "test/test_input/hello_arm.ks";
    char* output_file = "hello_arm_ks.bin";
    asm_file(input_file,output_file);

}
static void test_asm_raw_oneword(){
    char* input = ".raw \"test\"";
    int expect = 0x74736574;
    
    init_emitter(&g_emitter);
    asm_line(input,&g_emitter);

    assert(expect == g_emitter.words[0]);
}
static void test_asm_raw_str(){
    char* input = ".raw \"hello world\\n\"";
    int expect1 = 0x6C6C6568; 
    int expect2 = 0x6F77206F; 
    int expect3 = 0x0A646C72; 
   
    init_emitter(&g_emitter);
    asm_line(input,&g_emitter);

    assert(expect1 == g_emitter.words[0]);
    assert(expect2 == g_emitter.words[1]);
    assert(expect3 == g_emitter.words[2]);
}
static void test_asm_ldr_label_firstpass(){
    char* input = "ldr r1,=message";
    int expect = 0xE59F1000;

    init_emitter(&g_emitter);
    clear_unresolved_items();
    init_label_tree();
    struct Unresolved_item unresolved_item;
    assert(0 == get_unresolved_item(&unresolved_item));
    int expect_emitter_pos = g_emitter.pos;
    asm_line(input,&g_emitter);

    assert(expect == g_emitter.words[0]);
    assert(1 == get_unresolved_item(&unresolved_item));
    assert(to_label_symbol("message",5) == unresolved_item.label_symbol);
    assert(to_mnemonic_symbol("ldr",1) == unresolved_item.mnemonic_symbol);
    assert(expect_emitter_pos == unresolved_item.emitter_pos);
}
static void test_asm_file_loop(){
/*
    動作確認のためファイルに出力するだけの関数です。
    この関数内では、結果の判定を行いません。

    input:  hello_loop.s相当のファイルhello_loop.ksを読み込み、
    expect: hello_loop.sをARMアセンブラでアセンブルしたバイナリと、同等のバイナリを書き出す。
*/
    char* input_file = "test/test_input/hello_loop.ks";
    char* output_file = "hello_loop_ks.bin";
    asm_file(input_file,output_file);

}
static void test_asm_ldrb(){
    char* input = "ldrb r3,[r1]";
    int expect = 0xE5D13000; 

    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_asm_cmp(){
    char* input = "cmp r3,#12";
    int expect = 0xE353000C; 

    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_asm_cmp2(){
    char* input = "cmp r3,r4";
    int expect = 0xE1530004; 

    int actual = asm_one(input);

    assert(expect == actual);
}
static void test_asm_bne_firstpass(){
    char* input = "bne label";
    int expect = 0x1A000000;

    init_emitter(&g_emitter);
    init_label_tree();
    struct Unresolved_item unresolved_item;
    assert(0 == get_unresolved_item(&unresolved_item));
    int expect_emitter_pos = g_emitter.pos;
    asm_line(input,&g_emitter);

    assert(expect == g_emitter.words[0]);
    assert(1 == get_unresolved_item(&unresolved_item));
    assert(to_label_symbol("label",5) == unresolved_item.label_symbol);
    assert(to_mnemonic_symbol("bne",3) == unresolved_item.mnemonic_symbol);
    assert(expect_emitter_pos == unresolved_item.emitter_pos);
}
static void test_asm_add(){
    char* input = "add r1, r1, #1";
    int expect = 0xE2811001; 

    int actual = asm_one(input);

    assert(expect == actual);
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
    test_asm_raw_oneword();
    test_asm_raw_str();
    test_asm_ldr_label_firstpass(); 
    test_asm_file_loop(); 
    test_asm_ldrb();
    test_asm_cmp();
    test_asm_cmp2();
    test_asm_bne_firstpass();
    test_asm_add();
}

static void unittests(){
    cl_getline_unittests();
    cl_binarytree_unittests();
    dict_unittests();
    parser_unittests();

    init_mnemonic_symbols();
    asm_unittests();
}

int main(int argc, char* argv[]){
    init_mnemonic_symbols();
    if(3 == argc){
        asm_file(argv[1],argv[2]);
    }
    unittests();
    return 0;
}

