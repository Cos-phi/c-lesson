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
/*
struct Nodeで表現される3種類の要素で二分木を構成します。
[NODE] ノード  name,valueに値を持ち、left,rightに一つ以上の[LEAF]または[NODE]のポインタを持ちます。
[LEAF] リーフ  name,valueを値を持ち、left,rightに[]のポインタを持ちます。
[]    空ノード nameがNULL

4つの項目からなる二分木の例を以下に示します

     [NODE]
      /  \
 [NODE]  [LEAF]
  /  \     /  \
[] [LEAF] []  []
    /  \
   []  []

*/

struct Node mnemonic_root;
struct Node label_root;

int mnemonic_id = 1;
int label_id = 10000;


int search_symbol(char *str, struct Node **inout_node) {
/*
    文字列と、ツリーのルートのノードのポインタを受け取って、ツリーをたどります。
    ツリーに見つかったら1を、ツリーになかった場合は0を返します。
    ノードのポインタは、その文字列が 入っているノード/入るべき空ノード に更新されます。
*/
    struct Node* cur_node = *inout_node;
    while(NULL != cur_node->name){
        int cur_strcmp = strcmp(str,cur_node->name);
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

void init_empty_node(struct Node** empty_node){
    *empty_node = (struct Node*)malloc(sizeof(struct Node));
    (*empty_node)->name = NULL;
}

void set_new_node(char *str, int len, struct Node* new_node, int* id){
    new_node->name = (char*)malloc(sizeof(char)*(len+1));
    strcpy(new_node->name,str);
    new_node->value = (*id)++;
    init_empty_node(&(new_node->left));
    init_empty_node(&(new_node->right));
}

int to_symbol(char *str, int len, struct Node* cur_node, int* id) {
/*
    文字列とノード（ルート）を受け取って、ノード以下のツリーにおけるvalueを返します。
    ツリーになかった場合は追加してvalueを返します。
*/
    if( 0 == search_symbol(str, &cur_node) ){ 
        set_new_node(str,len,cur_node,id);
    }
    return cur_node->value;
}

int to_mnemonic_symbol(char *str, int len) {
    return to_symbol(str,len,&mnemonic_root,&mnemonic_id);
}

int to_label_symbol(char *str, int len) {
    return to_symbol(str,len,&label_root,&label_id);
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

    assert( 1 == to_mnemonic_symbol("aja",3));
    assert( 2 == to_mnemonic_symbol("mue",3));
    assert( 3 == to_mnemonic_symbol("meu",3));
}
static void test_func_to_mnemonic_symbol_2(){
    char* input1 = "aja";
    char* input2 = "mue";
    char* input3 = "meu";
    char* input4 = "muemue";
    char* input5 = "aaaaaa";
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

    assert( 1 == to_mnemonic_symbol("aja",3));
    assert( 2 == to_mnemonic_symbol("mue",3));
    assert( 3 == to_mnemonic_symbol("meu",3));
    assert( 4 == to_mnemonic_symbol("muemue",6));
    assert( 5 == to_mnemonic_symbol("aaaaaa",6));
    assert( 6 == to_mnemonic_symbol("mochimochi",10));
}
static void test_func_to_label_symbol(){
    char* input1 = "aja";
    char* input2 = "mue";
    char* input3 = "meu";
/*
    expect: to_label_symbolに同じ文字列を与えたとき、返り値のvalueが一致する
*/
    int value1 = to_label_symbol(input1,3);
    int value2 = to_label_symbol(input2,3);
    int value3 = to_label_symbol(input3,3);

    assert( 10000 == to_label_symbol("aja",3));
    assert( 10001 == to_label_symbol("mue",3));
    assert( 10002 == to_label_symbol("meu",3));
}

void cl_binarytree_unittests(){
    test_func_to_mnemonic_symbol();
    test_func_to_mnemonic_symbol_2();
    test_func_to_label_symbol();
}
