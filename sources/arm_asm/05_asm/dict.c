#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct KeyValue {
    int key;
    int value;
};
static int dict_pos = 0;
static struct KeyValue dict_array[64];

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
 


int address_put(int key, int value){
    dict_put(key,value);
}
int address_get(int key, int* out_value){
    return dict_get(key,out_value);
}


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