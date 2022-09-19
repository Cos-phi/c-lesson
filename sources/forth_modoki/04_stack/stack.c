#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


enum LexicalType {
    NUMBER,
    SPACE,
    EXECUTABLE_NAME,
    LITERAL_NAME,
    OPEN_CURLY,
    CLOSE_CURLY, 
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    enum LexicalType ltype;
    union {
        int number;
        char onechar;
        char *name;
    } u;
};

#define NAME_SIZE 256
#define STACK_SIZE 1024

static char stack[NAME_SIZE*STACK_SIZE];
static int cur_stack_pos = 0;
static int stack_pos_array[STACK_SIZE];
static enum LexicalType stack_ltype_array[STACK_SIZE];
static int stack_index = 0;

void stack_push(struct Token *input_token) {
    char *input_str;
    input_str = (char*)malloc(NAME_SIZE);
    if (NUMBER == input_token->ltype){
        sprintf(input_str,"%d",input_token->u.number);
    }else if (LITERAL_NAME == input_token->ltype) {
        input_str = input_token->u.name;
    }

    int input_pos = 0;
    while('\0' != input_str[input_pos]){
        stack[cur_stack_pos++] = input_str[input_pos++];
    }

    stack_index++;
    stack_pos_array[stack_index] = cur_stack_pos;
    stack_ltype_array[stack_index] = input_token->ltype;

}

void stack_pop(struct Token *out_token) {
    if(0 == stack_index){
        struct Token out_token = {UNKNOWN, {0}};
        return;
    }

    out_token->ltype = stack_ltype_array[stack_index];
    
    int end_pos = stack_pos_array[stack_index];
    int start_pos = stack_pos_array[--stack_index];
    int i = 0;
    char pop_str[NAME_SIZE];
    while(start_pos != end_pos){
        pop_str[i++] = stack[start_pos++];
    }
    pop_str[i] = '\0';

    if (NUMBER == out_token->ltype){
        out_token->u.number = str2int(pop_str);
    }else if (LITERAL_NAME == out_token->ltype) {
        out_token->u.name = pop_str;
    }
}

int isequal_token(struct Token *token1, struct Token *token2) {
    if (token1->ltype != token2->ltype){
        return 0;
    }else{
        switch(token1->ltype) {
            case NUMBER:
                if (token1->u.number == token2->u.number) return 1;
                else return 0;
            case LITERAL_NAME:
                if (0 == strcmp(token1->u.name, token2->u.name)) return 1;
                else return 0;
        }
    }
}

static void test_isequal_tokens_are_equal(){
    struct Token input_1;
    input_1.ltype = LITERAL_NAME;
    input_1.u.name = "abc";
    struct Token input_2;
    input_2.ltype = LITERAL_NAME;
    input_2.u.name = "abc";
    int expect = 1;

    int result = isequal_token(&input_1,&input_2);
    
    assert (expect == result);
}
static void test_one_pop(){
    struct Token expect = {UNKNOWN, {0}};

    struct Token output = {UNKNOWN, {0}};
    stack_pop(&output);

//    assert (1 == isequal_token(&expect,&output));
}
static void test_one_push(){}
static void test_one_push_one_pop(){}
static void test_two_push_two_pop(){}

static void unit_tests() {
    test_isequal_tokens_are_equal();
    test_one_pop();
//    test_one_push();
//    test_one_push_one_pop();
//    test_two_push_two_pop();
}

int main() {
    unit_tests();
    struct Token token;


    token.ltype = NUMBER;
    token.u.number = 12;
    stack_push(&token);

    token.ltype = LITERAL_NAME;
    token.u.name = "abc";
    stack_push(&token);

    token.ltype = LITERAL_NAME;
    token.u.name = "ABC";
    stack_push(&token);
    
    struct Token out_token;
    stack_pop(&out_token);
    printf("outname %s\n",out_token.u.name);

    stack_pop(&out_token);
    printf("outname %s\n",out_token.u.name);

    stack_pop(&out_token);
    printf("outnum %d\n",out_token.u.number);
} 

