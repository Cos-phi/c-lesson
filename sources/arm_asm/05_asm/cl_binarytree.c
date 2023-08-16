#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
struct Nodeで表現される3種類の要素で二分木を構成します。
[NODE] ノード（節点）   name,valueに値を持ち、left,rightに一つ以上の[LEAF]または[NODE]のポインタを持ちます。
[LEAF] リーフ（葉）     name,valueを値を持ち、left,rightに[]のポインタを持ちます。
[BUD]  バド（新芽）     Nodeとしてはmallocされていて、nameがNULLにセットされています。

4つの項目（ノードとリーフ）からなる二分木の例を以下に示します

         [NODE]
        //    \\
    [NODE]     [LEAF]
    //  \       /  \
 [LEAF] [BUD] [BUD] [BUD]
   /  \
[BUD] [BUD]

*/
struct Node {
    char *name;
    int value;
    struct Node *left;
    struct Node *right;
};

struct Node mnemonic_root;
struct Node label_root;

#define MNEMONIC_ID_START 1
#define LABEL_ID_START 10000
int mnemonic_id = MNEMONIC_ID_START;
int label_id = LABEL_ID_START;

int search_symbol(char *str, int len, struct Node **inout_node) {
//  文字列と、ツリーのルートのノードのポインタを受け取って、ツリーに見つかったら1を、ツリーになかった場合は0を返します。
//  ノードのポインタは、その文字列が 入っているノード・リーフ/入るべきバド に更新されます。
    struct Node* cur_node = *inout_node;
    while(NULL != cur_node->name){
        int cur_strcmp = strncmp(str,cur_node->name,len);
        if( 0 > cur_strcmp ){ 
            cur_node = cur_node->left;
        }else if( 0 < cur_strcmp ){ 
            cur_node = cur_node->right;
        }else{ // 0 == cur_strcmp 
            *inout_node = cur_node;
            return 1;
        }
    }
    *inout_node = cur_node;
    return 0;
}

void create_bud(struct Node** bud){
//  バドを生成します。
    *bud = (struct Node*)malloc(sizeof(struct Node));
    (*bud)->name = NULL;
}

void create_leaf(char *str, int len, struct Node* new_leaf, int* id){
//  文字列とバドを受け取って、バドをリーフにします。
    new_leaf->name = (char*)malloc(sizeof(char)*(len+1));
    strncpy(new_leaf->name,str,len); 
    new_leaf->value = (*id)++;
    create_bud(&(new_leaf->left));
    create_bud(&(new_leaf->right));
}

int to_symbol(char *str, int len, struct Node* cur_node, int* id) {
//  文字列とノード（ルート）を受け取って、ノード以下のツリーにおけるvalueを返します。
//  ツリーになかった場合は追加してvalueを返します。
    if( 0 == search_symbol(str, len, &cur_node) ){ 
        create_leaf(str,len,cur_node,id);
    }
    return cur_node->value;
}

int to_mnemonic_symbol(char *str, int len) {
//  文字列を受け取って、mnemonic treeにおけるvalueを返します。
//  ツリーになかった場合は追加してvalueを返します。
    return to_symbol(str,len,&mnemonic_root,&mnemonic_id);
}

int to_label_symbol(char *str, int len) {
//  文字列を受け取って、label treeにおけるvalueを返します。
//  ツリーになかった場合は追加してvalueを返します。
    return to_symbol(str,len,&label_root,&label_id);
}

void init_tree(struct Node* root_node){
//  ルートのノードを受け取って、ツリーをリセットします。
    root_node->name = NULL;
    root_node->value = 0;
    root_node->right = NULL;
    root_node->left = NULL;
}

void init_mnemonic_tree(){
    init_tree(&mnemonic_root);
    mnemonic_id = MNEMONIC_ID_START;
}

void init_label_tree(){
    init_tree(&label_root);
    label_id = LABEL_ID_START;
}

static void test_func_to_mnemonic_symbol(){
    char* input1 = "aja";
    char* input2 = "mue";
    char* input3 = "meu";
    /*
    expect: to_mnemonic_symbolに同じ文字列を与えたとき、返り値のvalueが一致する
    */  
    init_mnemonic_tree();
    int value1 = to_mnemonic_symbol(input1,3);
    int value2 = to_mnemonic_symbol(input2,3);
    int value3 = to_mnemonic_symbol(input3,3);

    assert( 1 == to_mnemonic_symbol("aja",3));
    assert( 2 == to_mnemonic_symbol("mue",3));
    assert( 3 == to_mnemonic_symbol("meu",3));
    assert( value1 == to_mnemonic_symbol("aja",3));
    assert( value2 == to_mnemonic_symbol("mue",3));
    assert( value3 == to_mnemonic_symbol("meu",3));
}
static void test_func_to_mnemonic_symbol_2(){
    char* input1 = "meu";
    char* input2 = "mue";
    char* input3 = "aja";
    char* input4 = "muemue";
    char* input5 = "aaaaaa";
    char* input6 = "mochimochi";
    /*
    expect: to_mnemonic_symbolに同じ文字列を与えたとき、返り値のvalueが一致する
    */
    init_mnemonic_tree();
    int value1 = to_mnemonic_symbol(input1,3);
    int value2 = to_mnemonic_symbol(input2,3);
    int value3 = to_mnemonic_symbol(input3,3);
    int value4 = to_mnemonic_symbol(input4,6);
    int value5 = to_mnemonic_symbol(input5,6);
    int value6 = to_mnemonic_symbol(input6,10);

    assert( 1 == to_mnemonic_symbol("meu",3));
    assert( 2 == to_mnemonic_symbol("mue",3));
    assert( 3 == to_mnemonic_symbol("aja",3));
    assert( 4 == to_mnemonic_symbol("muemue",6));
    assert( 5 == to_mnemonic_symbol("aaaaaa",6));
    assert( 6 == to_mnemonic_symbol("mochimochi",10));
    assert( value1 == to_mnemonic_symbol("meu",3));
    assert( value2 == to_mnemonic_symbol("mue",3));
    assert( value3 == to_mnemonic_symbol("aja",3));
    assert( value4 == to_mnemonic_symbol("muemue",6));
    assert( value5 == to_mnemonic_symbol("aaaaaa",6));
    assert( value6 == to_mnemonic_symbol("mochimochi",10));
}
static void test_func_to_mnemonic_symbol_3(){
    char* input1 = "aja";
    char* input2 = "mue";
    char* input3 = "meu";
    /*
    expect: to_mnemonic_symbolに同じ文字列（ただし、strに\0を含まず、文字数で指定したもの）を与えたとき、返り値のvalueが一致する
    */  
    init_mnemonic_tree();
    int value1 = to_mnemonic_symbol(input1,3);
    int value2 = to_mnemonic_symbol(input2,3);
    int value3 = to_mnemonic_symbol(input3,3);

    assert( 1 == to_mnemonic_symbol("aja",3));
    assert( 2 == to_mnemonic_symbol("mue",3));
    assert( 3 == to_mnemonic_symbol("meu",3));
    assert( value1 == to_mnemonic_symbol("ajaaaaa",3));
    assert( value2 == to_mnemonic_symbol("mueeeee",3));
    assert( value3 == to_mnemonic_symbol("meuuuuu",3));
}
static void test_func_to_label_symbol(){
    char* input1 = "aja";
    char* input2 = "mue";
    char* input3 = "meu";
    /*
    expect: to_label_symbolに同じ文字列を与えたとき、返り値のvalueが一致する
    */
    init_label_tree();
    int value1 = to_label_symbol(input1,3);
    int value2 = to_label_symbol(input2,3);
    int value3 = to_label_symbol(input3,3);

    assert( 10000 == to_label_symbol("aja",3));
    assert( 10001 == to_label_symbol("mue",3));
    assert( 10002 == to_label_symbol("meu",3));
    assert( value1 == to_label_symbol("aja",3));
    assert( value2 == to_label_symbol("mue",3));
    assert( value3 == to_label_symbol("meu",3));
}
static void test_func_to_label_symbol_toolongstr(){
    char* input1 = "ajadummydummy"; // lenより長い文字列が渡されたとき
    char* input2 = "mue";
    char* input3 = "meu";
    /*
    expect1: to_label_symbolに同じ文字列を与えたとき、返り値のvalueが一致する
    expect2: ASANのheap-buffer-overflowが発生しない
    */
    init_label_tree();
    int value1 = to_label_symbol(input1,3); // len = 3 
    int value2 = to_label_symbol(input2,3);
    int value3 = to_label_symbol(input3,3);

    assert( 10000 == to_label_symbol("aja",3));
    assert( 10001 == to_label_symbol("mue",3));
    assert( 10002 == to_label_symbol("meu",3));
    assert( value1 == to_label_symbol("aja",3));
    assert( value2 == to_label_symbol("mue",3));
    assert( value3 == to_label_symbol("meu",3));
}

void cl_binarytree_unittests(){
    test_func_to_mnemonic_symbol();
    test_func_to_mnemonic_symbol_2();
    test_func_to_mnemonic_symbol_3();
    test_func_to_label_symbol();
    test_func_to_label_symbol_toolongstr();
    
    init_mnemonic_tree();
    init_label_tree();
}
