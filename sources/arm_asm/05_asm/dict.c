#include "clesson.h"
#define DICT_ARRAY_SIZE 128

struct KeyValue {
    int key;
    int value;
};
static int dict_pos = 0;
static struct KeyValue dict_array[DICT_ARRAY_SIZE];

void dict_put(int key, int value){
    for(int i = 0; i < dict_pos; i++) {
        if( key == dict_array[i].key ){
            dict_array[i].value = value;
            return; 
        }
    }
    dict_array[dict_pos].key = key;
    dict_array[dict_pos].value = value;
    dict_pos++;
}

int dict_get(int key, int* out_value){
    for(int i = 0; i < dict_pos; i++) {
        if( key == dict_array[i].key ){
            *out_value = dict_array[i].value;;
            return 1;
        }
    }
    return 0;
}

void dict_print_all(){
    for(int i = 0; i < dict_pos; i++) {
        printf("key: %d, value: %d \n",dict_array[i].key, dict_array[i].value);
    }
}

void dict_clear(){
    dict_pos = 0;
}


/*
    アドレスの解決に必要な、ラベルとの対応関係を辞書にまとめます。
    ここでのアドレス（位置）は、emitter.posで表すものとします。
*/
void dict_emitter_pos_put(int label_symbol, int address){    
    dict_put(label_symbol,address);
}
int dict_emitter_pos_get(int label_symbol){
    int address;
    if( 1 == dict_get(label_symbol,&address) ){
        return address;
    }else{
        abort();
    }
}

/*
    ユニットテスト
*/
static void test_dict() {
    int input_key = 1001;
    int input_value = 2001;
    int expect_value = 2001;

    dict_put(input_key,input_value);

    int actual_value;
    dict_get(input_key,&actual_value);
    
    assert( expect_value == actual_value );
}

void dict_unittests(){
    test_dict();
    dict_clear();
}