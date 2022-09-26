#include "clesson.h"

static int dict_pos = 0;
static struct KeyValue dict_array[1024];

void dict_put(char* key, struct Token *token){
    for(int i = 0; i < dict_pos; i++) {
        if( streq( key, dict_array[i].key ) ) {
            dict_array[i].value = *token;
            return; 
        }
    }
    dict_array[dict_pos].key = key;
    dict_array[dict_pos].value = *token;
    dict_pos++;
}

int dict_get(char* key, struct Token *out_token){
    for(int i = 0; i < dict_pos; i++) {
        if(streq(key, dict_array[i].key)) {
            *out_token = dict_array[i].value;
            return 1;
        }
    }
    return 0;
}

void dict_print_all(){
    for(int i = 0; i < dict_pos; i++) {
        printf("key: %s, value: %d \n",dict_array[i].key, dict_array[i].value.u.number);
    }
}

void dict_clear(){
    dict_pos = 0;
}

int isequal_keyvalue(struct KeyValue *keyvalue1, struct KeyValue *keyvalue2){
    if( keyvalue1->key != keyvalue2->key ){
        return 0;
    }else if( 1 != isequal_token( &(keyvalue1->value), &(keyvalue2->value) )){
        return 0;
    }else{
        return 1;
    }
}


static void test_def_and_add() {
    char *input = "/abc 12 def abc abc add";
    int expect = 24;

    cl_getc_set_src(input);
    eval();
    
    struct Token actual_token = {UNKNOWN, {0} };
    stack_pop(&actual_token);
    int actual = actual_token.u.number;

    assert(expect == actual);

}

static void test_def() {
    char *input = "/abc 12 def";
    struct Token expect_value; expect_value.ltype = NUMBER; expect_value.u.number = 12;

    cl_getc_set_src(input);
    eval();
    struct Token actual_token = {UNKNOWN, {0} };
    dict_get("abc",&actual_token);
    int actual = actual_token.u.number;
    
    assert(12 == actual);

}

static void test_isequal_keyvalue(){
    struct Token input1_value;  input1_value.ltype  = NUMBER; input1_value.u.number  = 123;
    struct KeyValue input1 = { "key1", input1_value};
    struct Token input2_value;  input2_value.ltype  = NUMBER; input2_value.u.number  = 123;
    struct KeyValue input2 = { "key1", input2_value};
    
    int ans = isequal_keyvalue(&input1,&input2);
    
    assert(1 == ans);
}

static void test_dict_put() {
    char* input_key = "key";
    struct Token input_value;  input_value.ltype  = NUMBER; input_value.u.number  = 123;
    struct Token expect_value; expect_value.ltype = NUMBER; expect_value.u.number = 123;
    struct KeyValue expect = {"key", expect_value};

    dict_put(input_key,&input_value);
    
    assert( isequal_keyvalue( &(dict_array[0]), &expect ) );
}

void unit_tests_dict(){
    test_isequal_keyvalue();
    dict_clear();
    test_dict_put();
    dict_clear();
    test_def();
    dict_clear();
    test_def_and_add();
}
