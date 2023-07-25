#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *name;
    int value;
    struct Node *left;
    struct Node *right;
};

struct Node mnemonic_root;
struct Node label_root;

int mnemonic_id = 1;
int label_id = 10000;


int search_symbol(char *str, struct Node **inout_node) {
/*
    文字列と、ツリーのルートのノードのポインタを受け取って、ツリーをたどります。
    ツリーに見つかったらvalueを返します。ツリーになかった場合は-1を返します。
    ノードはそのときのノードに更新されます
*/
    struct Node* cur_node = *inout_node;
    while(NULL != cur_node){
        int cur_strcmp = strcmp(str,cur_node->name);
        if( 0 == cur_strcmp ){
            *inout_node = cur_node;
            return cur_node->value;
        }else if( 0 > cur_strcmp && NULL != cur_node->left){ 
            cur_node = cur_node->left;
        }else if( 0 < cur_strcmp && NULL != cur_node->right){ 
            cur_node = cur_node->right;
        }else {
            *inout_node = cur_node;
            return -1;
        }
    }
}

int to_symbol(char *str, int len, struct Node* cur_node, int* id) {
/*
    文字列とノードを受け取って、ノード以下のツリーにおけるvalueを返します。
    ツリーになかった場合は追加してvalueを返します。
*/
    if( NULL == cur_node->name ){ 
        cur_node->name = (char*)malloc(sizeof(char)*(len+1));
        strcpy(cur_node->name,str);
        cur_node->value = *id++;
        cur_node->left = NULL;
        cur_node->right = NULL;
        return cur_node->value;
    }
    int value = search_symbol(str, &cur_node);
    if( -1 == value ){ //ツリーになかった場合
        struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
        new_node->name = (char*)malloc(sizeof(char)*(len+1));
        strcpy(new_node->name,str);
        new_node->value = *id++;
        new_node->left = NULL;
        new_node->right = NULL;

        int cur_strcmp = strcmp(str,cur_node->name);
        if( 0 > cur_strcmp ){ 
            cur_node->left = new_node;
        }else if( 0 < cur_strcmp ){ 
            cur_node->right = new_node;
        }else {// 0 == cur_strcmp
            abort();
        }
        /*↑ここ、先に呼び出したsearch_sybolの中で既にやった比較をもう一回やっているのが
        かっこよくありませんのよね。どうにかできる（するべき？）ものかしら？
        */
        return new_node->value;
    }else{
        return value;
    }
}

int to_mnemonic_symbol(char *str, int len) {
/*
    文字列を受け取って、mnemonicのツリーにおけるvalueを返します。
*/
    int value = to_symbol(str,len,&mnemonic_root,&mnemonic_id);
}

static void test_func_to_mnemonic_symbol(){
    char* input1 = "aja";
    char* input2 = "mue";
    char* input3 = "meu";
/*
    expect: to_mnemonic_symbolに同じ文字列を与えたとき、返り値のvalueが一致する
*/
    int value1 = to_mnemonic_symbol(input1,3);
    int value2 = to_mnemonic_symbol(input2,3);
    int value3 = to_mnemonic_symbol(input3,3);

    assert( value1 == to_mnemonic_symbol("aja",3));
    assert( value2 == to_mnemonic_symbol("mue",3));
    assert( value3 == to_mnemonic_symbol("meu",3));
}
static void test_func_to_mnemonic_symbol_2(){
    char* input1 = "aja";
    char* input2 = "mue";
    char* input3 = "meu";
    char* input4 = "muemue";
    char* input5 = "meuuuu";
    char* input6 = "mochimochi";
/*
    expect: to_mnemonic_symbolに同じ文字列を与えたとき、返り値のvalueが一致する
*/
    int value1 = to_mnemonic_symbol(input1,3);
    int value2 = to_mnemonic_symbol(input2,3);
    int value3 = to_mnemonic_symbol(input3,3);
    int value4 = to_mnemonic_symbol(input4,6);
    int value5 = to_mnemonic_symbol(input5,6);
    int value6 = to_mnemonic_symbol(input6,10);

    assert( value1 == to_mnemonic_symbol("aja",3));
    assert( value2 == to_mnemonic_symbol("mue",3));
    assert( value3 == to_mnemonic_symbol("meu",3));
    assert( value4 == to_mnemonic_symbol("muemue",6));
    assert( value5 == to_mnemonic_symbol("meuuuu",6));
    assert( value6 == to_mnemonic_symbol("mochimochi",10));
}

void cl_binarytree_unittests(){
    test_func_to_mnemonic_symbol();
    test_func_to_mnemonic_symbol_2();
}
