#include "element.h"
#include "clesson.h"

struct Node {
    char *key;
    struct Element value;
    struct Node *next;
};

struct Node* eval_dict[TABLE_SIZE];
struct Node* compile_dict[TABLE_SIZE];

int streq(char *s1, char *s2){
    if( 0 == strcmp(s1,s2) ){
        return 1;
    }else{
        return 0;
    } 
}

void update_or_insert_list(struct Node *cur_node, char* key, struct Element *value){
    struct Node *prev_node;
    while( NULL != cur_node ){
        if( streq(cur_node->key,key) ){
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
    cur_node->next = NULL;
    prev_node->next = cur_node;
}

void dict_put(char* key, struct Element *value){
    int idx = hash(key);
    struct Node *head = eval_dict[idx];
    if( NULL == head ) {
        head = malloc(sizeof(struct Node));
        head->next = NULL;
        head->key = key;
        head->value = *value;
        eval_dict[idx] = head;
        return;
    }
    update_or_insert_list(head, key, value);
}

static int dict_get_common(struct Node* table, char* key, struct Element *out_element){
    int idx = hash(key);
    struct Node *cur_node = table[idx];
    while( cur_node != NULL ){
        if( streq(key,cur_node->key) ){
            *out_element = cur_node->value;
            return 1;
        }
        cur_node = cur_node->next;
    }
    return 0;
}

int dict_get(char* key, struct Element *out_element){
    return dict_get_common(eval_dict,key,out_element);
}

void dict_print_all(){
    int i;
    for(i = 0; i < TABLE_SIZE ; i++) {
        struct Node *cur_node = eval_dict[i];
        printf("hash %d :",i);
        while( cur_node != NULL ){
            printf(" %s => %d ",cur_node->key, cur_node->value.u.number);
            cur_node = cur_node->next;
        }
        printf("\n");
    }
}

void dict_clear(){
    int i;
    for(i = 0; i < TABLE_SIZE ; i++) {
        eval_dict[i] = NULL;
    }
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
    char* input_key  = "muee";
    char* input_key2 = "umee";
    char* input_key3 = "meeu";
    char* input_key4 = "aja";
    char* input_key5 = "emeu";
    struct Element input_value;  input_value.etype  = ELEMENT_NUMBER; input_value.u.number  = 1;
    struct Element input_value2;  input_value2.etype  = ELEMENT_NUMBER; input_value2.u.number  = 2;
    struct Element input_value3;  input_value3.etype  = ELEMENT_NUMBER; input_value3.u.number  = 4;
    struct Element input_value4;  input_value4.etype  = ELEMENT_NUMBER; input_value4.u.number  = 32;
    struct Element input_value5;  input_value5.etype  = ELEMENT_NUMBER; input_value5.u.number  = 15;
    struct Element expect_value; expect_value.etype = ELEMENT_NUMBER; expect_value.u.number = 4;
    struct Element actual_value = {ELEMENT_UNKNOWN, {0}};

    dict_put(input_key,&input_value);
    dict_put(input_key2,&input_value2);
    dict_put(input_key3,&input_value3);
    dict_put(input_key4,&input_value4);
    dict_put(input_key5,&input_value5);

    dict_get(input_key3,&actual_value);
    
    assert( actual_value.u.number == expect_value.u.number );
}

static void unit_tests_dict(){
    dict_clear();
    test_dict_put();
    dict_clear();
    test_dict_put2();
    dict_clear();
}

#if 0
void main(){
    unit_tests_dict();
}
#endif
