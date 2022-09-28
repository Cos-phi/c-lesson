#include "clesson.h"


void add_nums(){
    struct Token num1;
    struct Token num2;
    stack_pop(&num1);
    stack_pop(&num2);

    struct Token sum = {NUMBER, {0} };
    sum.u.number = num1.u.number + num2.u.number;

    stack_push(&sum);
}

void def(){
    struct Token val;
    struct Token literal_name;
    stack_pop(&val);
    stack_pop(&literal_name);
    assert( literal_name.ltype == LITERAL_NAME);

    dict_put(literal_name.u.name, &val);
}

void eval() {
    struct Token token = {UNKNOWN, {0} };
    int ch = EOF;
    do {
        ch = parse_one(ch, &token);
        if(token.ltype != UNKNOWN) {
            struct Token ref_token = {UNKNOWN, {0} };
            switch(token.ltype) {
                case NUMBER:
                    stack_push(&token);
                    break;
                case SPACE:
                case OPEN_CURLY:
                case CLOSE_CURLY:
                    break;
                case EXECUTABLE_NAME:
                    if( streq("add", token.u.name) ){
                        add_nums();
                    }else if( streq("def", token.u.name) ){
                        def();
                    }else if( dict_get(token.u.name,&ref_token) ){
                        stack_push(&ref_token);
                    }
                    break;
                case LITERAL_NAME:
                    stack_push(&token);
                    break;
                default:
                    break;
            }
        }
    }while(ch != EOF);

}

static void test_def_and_add() {
    char *input = "/abc 12 def abc abc add";
    int expect = 24;

    cl_getc_set_src(input);
    eval();
    
    struct Token actual_token = {UNKNOWN, {0} };
    stack_pop(&actual_token);
    int actual = actual_token.u.number;

    assert(expect == actual);

}

static void test_def() {
    char *input = "/abcd 12 def";
    cl_getc_set_src(input);
    struct Token expect_value; expect_value.ltype = NUMBER; expect_value.u.number = 12;
    struct Token actual_token = {UNKNOWN, {0} };

    eval();
    dict_get("abcd",&actual_token);
    int actual = actual_token.u.number;
    
    assert(12 == actual);

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
    unit_tests_dict();
    test_def();
    dict_clear();
    test_def_and_add();

    return 0;
}
