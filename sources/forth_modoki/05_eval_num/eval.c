#include "clesson.h"

void eval() {
    struct Token token = {UNKNOWN, {0} };
    int ch = EOF;
    do {
        ch = parse_one(ch, &token);
        if(token.ltype != UNKNOWN) {
            switch(token.ltype) {
                case NUMBER:
                //    printf("num: %d\n", token.u.number);
                    stack_push(&token);
                    break;
                case SPACE:
                //    printf("space!\n");
                    break;
                case OPEN_CURLY:
                //    printf("Open curly brace '%c'\n", token.u.onechar);
                    break;
                case CLOSE_CURLY:
                //    printf("Close curly brace '%c'\n", token.u.onechar);
                    break;
                case EXECUTABLE_NAME:
                //    printf("EXECUTABLE_NAME: %s\n", token.u.name);
                    break;
                case LITERAL_NAME:
                //    printf("LITERAL_NAME: %s\n", token.u.name);
                    break;

                default:
                //    printf("Unknown type %d\n", token.ltype);
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

    /* TODO: write code to pop stack top element */
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

    /* TODO: write code to pop stack top and second top element */
    int actual1 = 0;
    int actual2 = 0;

    assert(expect1 == actual1);
    assert(expect2 == actual2);
}


static void test_eval_num_add() {
    char *input = "1 2 add";
    int expect = 3;

    cl_getc_set_src(input);

    eval();

    /* TODO: write code to pop stack top element */
    int actual = 0;
    assert(expect == actual);
}


int main() {
    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_add();

    return 0;
}
