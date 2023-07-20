#include <stdio.h>
#include <assert.h>
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

int to_symbol(char *str, int len) {
/*
    文字列を受け取って、mnemonicのツリーにおけるvalueを返します。
    ツリーになかった場合は追加してvalueを返します。
*/
    struct Node* cur_node = &mnemonic_root;
    int value = search_mnemonic_symbol(str, cur_node);


    if( -1 == value ){ //ツリーになかった場合
        return 0;
    }else{
        return value; //新しいvalue;
    }
}

int search_symbol(char *str, struct Node *inout_node) {
/*
    文字列と、ツリーのルートのノードのポインタを受け取って、ノードを更新しながらツリーをたどります。
    ツリーに見つかったらvalueを返します。ツリーになかった場合は、-1を返します。
*/
    struct Node *inout_node = inout_node;
    do{
        int cur_strcmp = strcmp(str,inout_node->name);
        if( 0 > cur_strcmp ){ 
            inout_node = inout_node->left;
        }else if( 0 < cur_strcmp ){ 
            inout_node = inout_node->right;
        }else {// 0 == cur_strcmp
            return inout_node->value;
        }
    }while(NULL != inout_node);
    return -1;
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
#if 1
int main(){
    test_func_to_mnemonic_symbol();
    return 0;
}
#endif