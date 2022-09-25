#include "clesson.h"

int streq(char *s1,char *s2){
    if( 0 == strcmp(s1,s2) ){
        return 1;
    }else{
        return 0;
    } 
}

static int dict_pos = 0;

struct KeyValue {
    char *key;
    struct Token value;
};
static struct KeyValue dict_array[1024];

void dict_put(char* key, struct Token *token){
    int i = 0;
    while( i < dict_pos){
        if( streq( key, dict_array[i].key ) ) {
            dict_array[i].value = *token;
            break; //ここでreturnすれば↓のif要らなくなるかしら
        }
        i++;
    }
    if( dict_pos == i ){
        dict_array[dict_pos].key = key;
        dict_array[dict_pos].value = *token;
        dict_pos++;
    }
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

int isequal_keyvalue(struct KeyValue *keyvalue1, struct KeyValue *keyvalue2){
    if( keyvalue1->key != keyvalue2->key ){
        return 0;
    }else if( 1 != isequal_token( &(keyvalue1->value), &(keyvalue2->value) )){
        return 0;
    }else{
        return 1;
    }
}

void add_nums(){
    struct Token num1;
    struct Token num2;
    stack_pop(&num1);
    stack_pop(&num2);

    struct Token sum = {NUMBER, {0} };
    sum.u.number = num1.u.number + num2.u.number;

    stack_push(&sum);
}

void eval() {
    struct Token token = {UNKNOWN, {0} };
    int ch = EOF;
    do {
        ch = parse_one(ch, &token);
        if(token.ltype != UNKNOWN) {
            switch(token.ltype) {
                case NUMBER:
                    stack_push(&token);
                    break;
                case SPACE:
                case OPEN_CURLY:
                case CLOSE_CURLY:
                    break;
                case EXECUTABLE_NAME:
                    if( 1 == streq(token.u.name,"add")){
                        add_nums();
                    }
                    break;
                case LITERAL_NAME:
                    stack_push(&token);
                    break;
                default:
                    break;
            }
        }
    }while(ch != EOF);

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

static void test_eval_num_one() {
    char *input = "123";
    int expect = 123;

    cl_getc_set_src(input);

    eval();

    struct Token actual_token = {UNKNOWN, {0} };
    stack_pop(&actual_token);
    int actual = actual_token.u.number;

    assert(expect == actual);

}

static void test_eval_num_two() {
    char *input = "123 456";
    int expect1 = 456;
    int expect2 = 123;

    cl_getc_set_src(input);

    eval();

    struct Token actual_token1 = {UNKNOWN, {0} };
    struct Token actual_token2 = {UNKNOWN, {0} };
    stack_pop(&actual_token1);
    stack_pop(&actual_token2);
    int actual1 = actual_token1.u.number;
    int actual2 = actual_token2.u.number;

    assert(expect1 == actual1);
    assert(expect2 == actual2);
}


static void test_eval_num_add() {
    char *input = "1 2 add";
    int expect = 3;

    cl_getc_set_src(input);

    eval();

    struct Token actual_token = {UNKNOWN, {0} };
    stack_pop(&actual_token);
    int actual = actual_token.u.number;

    assert(expect == actual);
}

static void test_eval_num_add2() {
    char *input = "1 2 3 add add 4 5 6 7 8 9 add add add add add add";
    int expect = 45;

    cl_getc_set_src(input);

    eval();

    struct Token actual_token = {UNKNOWN, {0} };
    stack_pop(&actual_token);
    int actual = actual_token.u.number;

    assert(expect == actual);
}

int main() {
    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_add();
    test_eval_num_add2();
    test_isequal_keyvalue();
    test_dict_put();

    return 0;
}
