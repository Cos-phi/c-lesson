#include <stdio.h>
#include <assert.h>
#include <string.h>

struct Node {
    char *name;
    int value;
    struct Node *left;
    struct Node *right;
};

struct Node Mnemonic_root;
struct Node label_root;

int mnemonic_id = 1;
int label_id = 10000;

int to_mnemonic_symbol(char *str, int len) {
/*
    文字列を受け取って、mnemonicのツリーにおけるvalueを返す。
    ツリーになかった場合は追加してvalueを返す
*/
    if(1/*ツリーにあったら*/){
        return 0; //そのノードのvalue;
    }else{
        //新しくノードをmnemonic_id++のvalueで追加
        return 0; //新しいvalue;
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
#if 1
int main(){
    test_func_to_mnemonic_symbol();
    return 0;
}
#endif