#include "clesson.h"

struct Node {
    char *key;
    struct Token value;
    struct Node *next;
};
#define TABLE_SIZE 1024
struct Node *array[TABLE_SIZE];


//static int dict_pos = 0;
//static struct KeyValue dict_array[1024];

int streq(char *s1,char *s2){
    if( 0 == strcmp(s1,s2) ){
        return 1;
    }else{
        return 0;
    } 
}

void update_or_insert_list(struct Node *head, char* key, struct Token *value){
    int idx = hash(key);
    do{
        if( head->key == key ){
            head->value = *value;
            return;
        }else if( NULL == head ){
            head = malloc(sizeof(struct Node));
            head->next = NULL;
            head->key = key;
            head->value = *value;
            return;
        }
        head = head->next;
    }while( NULL != head ); 
}

void dict_put(char* key, struct Token *value){
    int idx = hash(key);
    struct Node *head = array[idx];
    if( NULL == head ) {
        head = malloc(sizeof(struct Node));
        head->next = NULL;
        head->key = key;
        head->value = *value;
        array[idx] = head;
        return;
    }
    update_or_insert_list(head, key, value);
}

int dict_get(char* key, struct Token *out_token){
    dict_print_all();
    int idx = hash(key);
    struct Node *head = array[idx];
    while( head != NULL ){
        printf("head->key: '%s' val: %d \n",head->key, head->value.u.number);
        printf("key      : '%s' \n",key);
        assert( head->key == key );
        if( head->key == key ){
            *out_token = head->value;
            return 1;
        }
        head = head->next;
    }
    printf("key %s is not exist!\n",head->key);
    return 0;
}

void dict_print_all(){
    for(int i = 0; i < TABLE_SIZE ; i++) {
        struct Node *head = array[i];
        while( head != NULL ){
            printf("key: %s, value: %d \n",head->key, head->value.u.number);
            head = head->next;
        }
    }
}

void dict_clear(){
    for(int i = 0; i < TABLE_SIZE ; i++) {
        array[i] = NULL;
    }
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



static void test_isequal_keyvalue(){
    struct Token input1_value;  input1_value.ltype  = NUMBER; input1_value.u.number  = 123;
    struct KeyValue input1 = { "key1", input1_value};
    struct Token input2_value;  input2_value.ltype  = NUMBER; input2_value.u.number  = 123;
    struct KeyValue input2 = { "key1", input2_value};
    
    int ans = isequal_keyvalue(&input1,&input2);
    
    assert(1 == ans);
}

static void test_dict_put() {
    char* input_key = "hoge";
    struct Token input_value;  input_value.ltype  = NUMBER; input_value.u.number  = 123;
    struct Token expect_value; expect_value.ltype = NUMBER; expect_value.u.number = 123;
    struct Token actual_value = {UNKNOWN, {0}};

    dict_put(input_key,&input_value);
    dict_get(input_key,&actual_value);
    
    
    assert( actual_value.u.number == input_value.u.number );
}

static void test_dict_put2() {
    char* input_key = "mue-";
    struct Token input_value;  input_value.ltype  = NUMBER; input_value.u.number  = 123;
    struct Token expect_value; expect_value.ltype = NUMBER; expect_value.u.number = 123;
    struct Token actual_value = {UNKNOWN, {0}};

    dict_put(input_key,&input_value);
    dict_get(input_key,&actual_value);
    
    
    assert( actual_value.u.number == input_value.u.number );
}

void unit_tests_dict(){
    test_isequal_keyvalue();
    dict_clear();
    test_dict_put();
    dict_clear();
    test_dict_put2();
    dict_clear();
}
