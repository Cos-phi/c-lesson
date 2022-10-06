#include "element.h"
#include "clesson.h"


struct Node {
    char *key;
    struct Element value;
    struct Node *next;
};
struct Node *array[TABLE_SIZE];

int streq(char *s1,char *s2){
    if( 0 == strcmp(s1,s2) ){
        return 1;
    }else{
        return 0;
    } 
}

void update_or_insert_list(struct Node *cur_node, char* key, struct Element *value){
    struct Node *prev_node;
    while( NULL != cur_node ){
        if( cur_node->key == key ){
            cur_node->value = *value;
            return;
        }
        prev_node = cur_node;
        cur_node = cur_node->next;
    }
    assert( NULL == cur_node );
    cur_node = malloc(sizeof(struct Node));
    cur_node->key = key;
    cur_node->value = *value;
    prev_node->next = cur_node;
}

void dict_put(char* key, struct Element *value){
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

int dict_get(char* key, struct Element *out_element){
    int idx = hash(key);
    struct Node *cur_node = array[idx];
    while( cur_node != NULL ){
        if( streq(key,cur_node->key) ){
            *out_element = cur_node->value;
            return 1;
        }
        cur_node = cur_node->next;
    }
    return 0;
}

void dict_print_all(){
    for(int i = 0; i < TABLE_SIZE ; i++) {
        struct Node *cur_node = array[i];
        printf("hash %d :",i);
        while( cur_node != NULL ){
            printf(" %s => %d ",cur_node->key, cur_node->value.u.number);
            cur_node = cur_node->next;
        }
        printf("\n");
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
    }else if( 1 != isequal_element( &(keyvalue1->value), &(keyvalue2->value) )){
        return 0;
    }else{
        return 1;
    }
}



static void test_isequal_keyvalue(){
    struct Element input1_value;  input1_value.etype  = ELEMENT_NUMBER; input1_value.u.number  = 123;
    struct KeyValue input1 = { "key1", input1_value};
    struct Element input2_value;  input2_value.etype  = ELEMENT_NUMBER; input2_value.u.number  = 123;
    struct KeyValue input2 = { "key1", input2_value};
    
    int ans = isequal_keyvalue(&input1,&input2);
    
    assert(1 == ans);
}

static void test_dict_put() {
    char* input_key = "hoge";
    struct Element input_value;  input_value.etype  = ELEMENT_NUMBER; input_value.u.number  = 123;
    struct Element expect_value; expect_value.etype = ELEMENT_NUMBER; expect_value.u.number = 123;
    struct Element actual_value = {ELEMENT_UNKNOWN, {0}};

    dict_put(input_key,&input_value);
    dict_get(input_key,&actual_value);
    
    
    assert( actual_value.u.number == input_value.u.number );
}

static void test_dict_put2() {
    char* input_key  = "mue-";
    char* input_key2 = "ume-";
    char* input_key3 = "me-u";
    char* input_key4 = "aja";
    struct Element input_value;  input_value.etype  = ELEMENT_NUMBER; input_value.u.number  = 1;
    struct Element input_value2;  input_value2.etype  = ELEMENT_NUMBER; input_value2.u.number  = 2;
    struct Element input_value3;  input_value3.etype  = ELEMENT_NUMBER; input_value3.u.number  = 4;
    struct Element input_value4;  input_value4.etype  = ELEMENT_NUMBER; input_value4.u.number  = 32;
    struct Element expect_value; expect_value.etype = ELEMENT_NUMBER; expect_value.u.number = 2;
    struct Element actual_value = {ELEMENT_UNKNOWN, {0}};

    dict_put(input_key,&input_value);
    dict_put(input_key2,&input_value2);
    dict_put(input_key3,&input_value3);
    dict_put(input_key4,&input_value4);

    dict_get(input_key2,&actual_value);
    
    assert( actual_value.u.number == expect_value.u.number );
}

void unit_tests_dict(){
    test_isequal_keyvalue();
    dict_clear();
    test_dict_put();
    dict_clear();
    test_dict_put2();
    dict_clear();
}
