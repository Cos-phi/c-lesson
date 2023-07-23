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

int to_mnemonic_symbol(char *str, int len) {
/*
    文字列を受け取って、mnemonicのツリーにおけるvalueを返します。
    ツリーになかった場合は追加してvalueを返します。
*/
    if( NULL == mnemonic_root.name ){ 
        mnemonic_root.name = (char*)malloc(sizeof(char)*(len+1));
        strcpy(mnemonic_root.name,str);
        mnemonic_root.value = mnemonic_id++;
        mnemonic_root.left = NULL;
        mnemonic_root.right = NULL;
        return mnemonic_root.value;
    }
    struct Node* cur_node = &mnemonic_root;
    int value = search_symbol(str, &cur_node);
    if( -1 == value ){ //ツリーになかった場合
        struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
        new_node->name = (char*)malloc(sizeof(char)*(len+1));
        strcpy(new_node->name,str);
        new_node->value = mnemonic_id++;
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

void cl_binarytree_unittests(){
    test_func_to_mnemonic_symbol();
}
