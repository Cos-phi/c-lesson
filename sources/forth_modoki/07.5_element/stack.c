#include "clesson.h"

static struct Token stack[STACK_SIZE];
static int stack_index = 0;

void stack_push(struct Token *input_token) {
    stack_index++;
    stack[stack_index].ltype = input_token->ltype;
    switch(input_token->ltype){
        case NUMBER:
            stack[stack_index].u.number = input_token->u.number;
            break;
        case EXECUTABLE_NAME:
        case LITERAL_NAME:
            stack[stack_index].u.name = input_token->u.name;
            break;
        case SPACE:
        case OPEN_CURLY:
        case CLOSE_CURLY:
        case END_OF_FILE:
            stack[stack_index].u.onechar = input_token->u.onechar;
            break;
        case UNKNOWN:
            break;
    }
}

void stack_pop(struct Token *out_token) {
    if(0 == stack_index){
        struct Token out_token = {UNKNOWN, {0}};
        return;
    }
    out_token->ltype = stack[stack_index].ltype ;
    switch(stack[stack_index].ltype){
        case NUMBER:
            out_token->u.number = stack[stack_index].u.number;
            break;
        case EXECUTABLE_NAME:
        case LITERAL_NAME:
            out_token->u.name = stack[stack_index].u.name;
            break;
        case SPACE:
        case OPEN_CURLY:
        case CLOSE_CURLY:
        case END_OF_FILE:
            out_token->u.onechar = stack[stack_index].u.onechar;
            break;
        case UNKNOWN:
            break;
    }
    stack_index--;
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
            case EXECUTABLE_NAME:
                if (0 == strcmp(token1->u.name, token2->u.name)) return 1;
                else return 0;
            case SPACE:
            case OPEN_CURLY:
            case CLOSE_CURLY:
            case END_OF_FILE:
                if (token1->u.onechar == token2->u.onechar) return 1;
                else return 0;
            default:
                return 1;
        }
    }
}

void stack_clear(){
    stack_index = 0;
}

void stack_print_all(){
    printf("stack");
    for( int i = 0; i < stack_index; i++){
        printf(" %d",stack[i].ltype); 
        switch(stack[stack_index].ltype){
            case NUMBER:
                printf("%d ",stack[stack_index].u.number);
                break;
            case EXECUTABLE_NAME:
            case LITERAL_NAME:
                printf("%s ",stack[stack_index].u.name);
                break;
            case SPACE:
            case OPEN_CURLY:
            case CLOSE_CURLY:
            case END_OF_FILE:
                printf("%c ",stack[stack_index].u.onechar);
                break;
            case UNKNOWN:
                break;
        }
    }
    printf("\n");
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

    stack_clear();
    stack_pop(&output);

    assert (1 == isequal_token(&expect,&output));
}

static void test_one_push(){
    struct Token input; input.ltype = NUMBER; input.u.number = 42;
    struct Token expect; expect.ltype = NUMBER; expect.u.number = 42;

    stack_clear();
    stack_push(&input);
    
    assert (1 == isequal_token(&expect,&stack[stack_index]));
}

static void test_one_push_one_pop(){
    struct Token input;  input.ltype  = NUMBER; input.u.number  = 33147;
    struct Token expect; expect.ltype = NUMBER; expect.u.number = 33147;
    struct Token output;

    stack_clear();
    stack_push(&input);
    stack_pop(&output);

    assert (1 == isequal_token(&expect,&output));
}
static void test_two_push_two_pop(){
    struct Token input1;  input1.ltype  = NUMBER;       input1.u.number  = -42;
    struct Token input2;  input2.ltype  = LITERAL_NAME; input2.u.name    = "paooonf";
    struct Token expect1; expect1.ltype = LITERAL_NAME; expect1.u.name   = "paooonf";
    struct Token expect2; expect2.ltype = NUMBER;       expect2.u.number = -42;
    struct Token output1;
    struct Token output2;

    stack_clear();
    stack_push(&input1);
    stack_push(&input2);
    stack_pop(&output1);
    stack_pop(&output2);

    assert (1 == isequal_token(&expect1,&output1));
    assert (1 == isequal_token(&expect2,&output2));

}

static void unit_tests() {
    test_isequal_tokens_are_equal();
    test_one_pop();
    test_one_push();
    test_one_push_one_pop();
    test_two_push_two_pop();
}

#if 0
int main() {
    unit_tests();
}
#endif
