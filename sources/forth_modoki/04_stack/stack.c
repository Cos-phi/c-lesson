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

//static char stack = (char *)malloc(sizeof(char)*NAME_SIZE*STACK_SIZE);
static char stack[NAME_SIZE*STACK_SIZE];
static int cur_stack_pos = 0;
static int stack_pos_array[STACK_SIZE];
static int stack_pos_array_pos = 0;

void stack_push(struct Token *input_token) {
    char* input_str;
    if (NUMBER == input_token->ltype){
        sprintf(input_str,"%d",input_token->u.number);
    }else if (LITERAL_NAME == input_token->ltype) {
        input_str = input_token->u.name;
    }

    int input_pos = 0;
    while('\0' != input_str[input_pos]){
        stack[cur_stack_pos++] = input_str[input_pos++];
    }

    stack_pos_array_pos++;
    stack_pos_array[stack_pos_array_pos] = cur_stack_pos;
}

void stack_pop(struct Token *out_token) {
    int end_pos = stack_pos_array[stack_pos_array_pos];
    stack_pos_array_pos--;
    int start_pos = stack_pos_array[stack_pos_array_pos];
    char pop_str[NAME_SIZE];
    int i = 0;
    while(start_pos != end_pos){
        pop_str[i++] = stack[start_pos++];
    }
    pop_str[i] = '\0';
    out_token->u.name = pop_str;
    //とりあえずnameで返してますけれど‥‥これだとLexicalTlTypeの情報が失われてますわねね
}

static void unit_tests() {

}

int main() {
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
} 
