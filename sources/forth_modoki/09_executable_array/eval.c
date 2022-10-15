#include "parser.h"
#include "element.h"
#include "clesson.h"

void def(){
    struct Element val;
    struct Element literal_name;
    stack_pop(&val);
    stack_pop(&literal_name);
    assert( literal_name.etype == ELEMENT_LITERAL_NAME);

    dict_put(literal_name.u.name, &val);
}

void add_op(){
    struct Element num1, num2;
    stack_pop(&num1);
    stack_pop(&num2);

    struct Element sum = {ELEMENT_NUMBER, {0} };
    sum.u.number = num1.u.number + num2.u.number;

    stack_push(&sum);
}

void sub_op(){
    struct Element num1, num2;
    stack_pop(&num1);
    stack_pop(&num2);

    struct Element sub = {ELEMENT_NUMBER, {0} };
    sub.u.number = num2.u.number - num1.u.number ;

    stack_push(&sub);
}

void mul_op(){
    struct Element num1, num2;
    stack_pop(&num1);
    stack_pop(&num2);

    struct Element mul = {ELEMENT_NUMBER, {0} };
    mul.u.number = num1.u.number * num2.u.number ;

    stack_push(&mul);
}

void div_op(){
    struct Element num1, num2;
    stack_pop(&num1);
    stack_pop(&num2);

    struct Element div = {ELEMENT_NUMBER, {0} };
    div.u.number = num2.u.number / num1.u.number ;

    stack_push(&div);
}

void register_primitive(char* name, void (*func)()) {
    struct Element primitive = {ELEMENT_C_FUNC, {0} };
    primitive.u.cfunc = func;
    dict_put(name, &primitive);
}

void register_primitives() {
    register_primitive("def", def);
    register_primitive("add", add_op);
    register_primitive("sub", sub_op);
    register_primitive("mul", mul_op);
    register_primitive("div", div_op);
}

struct Element create_num_element(int input){
    struct Element element = {ELEMENT_UNKNOWN, {0} };
    element.etype = ELEMENT_NUMBER;
    element.u.number = input;
    return element;
}
struct Element create_literal_element(char* input){
    struct Element element = {ELEMENT_UNKNOWN, {0} };
    element.etype = ELEMENT_LITERAL_NAME;
    element.u.name = input;
    return element;
}

void eval() {
    struct Token token = {TOKEN_UNKNOWN, {0} };
    int ch = EOF;
    do {
        ch = parse_one(ch, &token);
        if(token.ltype != TOKEN_UNKNOWN) {
            struct Element ref_element = {ELEMENT_UNKNOWN, {0} };
            switch(token.ltype) {
                case TOKEN_NUMBER:
                    ref_element = create_num_element(token.u.number);
                    stack_push(&ref_element);
                    break;
                case TOKEN_SPACE:
                case TOKEN_OPEN_CURLY:
                case TOKEN_CLOSE_CURLY:
                    break;
                case TOKEN_EXECUTABLE_NAME:
                    if( dict_get(token.u.name,&ref_element) ){
                        switch(ref_element.etype){
                            case ELEMENT_C_FUNC:
                                ref_element.u.cfunc();
                                break;
                            case ELEMENT_NUMBER:
                            case ELEMENT_LITERAL_NAME:
                                stack_push(&ref_element);
                                break;
                            case ELEMENT_EXECUTABLE_NAME:
                            case ELEMENT_UNKNOWN:
                                abort();
                        }
                    } else {
                        abort();
                    }
                    
                    break;
                case TOKEN_LITERAL_NAME:
                    ref_element = create_literal_element(token.u.name);
                    stack_push(&ref_element);
                    break;
                default:
                    break;
            }
        }
    }while(ch != EOF);
}

static void test_eval_def_and_add() {
    char *input = "/mue- 12 def /ume- 30 def ume- mue- add";
    int expect = 12 + 30;

    cl_getc_set_src(input);
    eval();
    
    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);

}

static void test_eval_def() {
    char *input = "/abcd 12 def";
    cl_getc_set_src(input);
    struct Element expect_value; expect_value.etype = ELEMENT_NUMBER; expect_value.u.number = 12;
    struct Element actual_element= {ELEMENT_UNKNOWN, {0} };

    eval();
    dict_get("abcd",&actual_element);
    int actual = actual_element.u.number;
    
    assert(12 == actual);

}

static void test_eval_num_one() {
    char *input = "123";
    int expect = 123;

    cl_getc_set_src(input);

    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_num_two() {
    char *input = "123 456";
    int expect1 = 456;
    int expect2 = 123;

    cl_getc_set_src(input);

    eval();

    struct Element actual_element1 = {ELEMENT_UNKNOWN, {0} };
    struct Element actual_element2 = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element1);
    stack_pop(&actual_element2);
    int actual1 = actual_element1.u.number;
    int actual2 = actual_element2.u.number;

    assert(expect1 == actual1);
    assert(expect2 == actual2);
}


static void test_eval_num_add() {
    char *input = "1 2 add";
    int expect = 3;

    cl_getc_set_src(input);

    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_num_add2() {
    char *input = "1 2 3 add add 4 5 6 7 8 9 add add add add add add";
    int expect = 45;

    cl_getc_set_src(input);

    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_num_sub() {
    char *input = "5 3 sub";
    int expect = 2;

    cl_getc_set_src(input);

    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_num_mul() {
    char *input = "8 3 mul";
    int expect = 24;

    cl_getc_set_src(input);

    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_num_div() {
    char *input = "7 2 div";
    int expect = 3;

    cl_getc_set_src(input);

    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_def_and_4_arithmetic_operators() {
    char *input = "/mue- 12 def /ume- 30 def 42 ume- mue- add mul ume- 3 div sub";
    int expect = (12 + 30) * 42 - 30 / 3;

    cl_getc_set_src(input);
    eval();
    
    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);

}

int main() {
    register_primitives();
    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_add();
    test_eval_num_add2();
    test_eval_def();

    dict_clear();
    register_primitives();
    test_eval_def_and_add();
    dict_clear();
    register_primitives();
    test_eval_num_sub(); 
    dict_clear();
    register_primitives();
    test_eval_num_mul(); 
    dict_clear();
    register_primitives();
    test_eval_num_div(); 
    

    dict_clear();
    register_primitives();
    test_eval_def_and_4_arithmetic_operators();
    return 0;
}
