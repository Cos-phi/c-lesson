#include "clesson.h"

int streq(char *s1,char *s2){
    if( 0 == strcmp(s1,s2) ){
        return 1;
    }else{
        return 0;
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
                    break;
                default:
                    break;
            }
        }
    }while(ch != EOF);

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

    return 0;
}
