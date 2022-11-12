#include "parser.h"
#include "element.h"
#include "clesson.h"

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
struct Element create_executable_element(char* input){
    struct Element element = {ELEMENT_UNKNOWN, {0} };
    element.etype = ELEMENT_EXECUTABLE_NAME;
    element.u.name = input;
    return element;
}

struct Element compile_exec_array(int* inout_ch){
    struct Element element = {ELEMENT_UNKNOWN, {0} };
    element.etype = ELEMENT_EXECUTABLE_NAME;

    struct Element cur_exec_array[MAX_NAME_OP_NUMBERS];
    int cur_index = 0;
    struct Token token = {TOKEN_UNKNOWN, {0} };
    do {
        *inout_ch = parse_one(*inout_ch, &token);
        struct Element ref_element = {ELEMENT_UNKNOWN, {0} };
        switch(token.ltype) {
            case TOKEN_NUMBER:
                ref_element = create_num_element(token.u.number);
                cur_exec_array[cur_index] = ref_element;
                cur_index++;
                break;
            case TOKEN_OPEN_CURLY:
                ref_element = compile_exec_array(inout_ch);
                cur_exec_array[cur_index] = ref_element;
                cur_index++;
                break;
            case TOKEN_CLOSE_CURLY:
                {
                struct ElementArray *arr = (struct ElementArray*)malloc( sizeof(struct ElementArray) + sizeof(struct Element)*(cur_index) );
                arr->len = cur_index;
                memcpy( arr->elements, cur_exec_array, sizeof(struct Element)*(cur_index));
                element.u.byte_codes = arr;
                return element;
                }
            case TOKEN_EXECUTABLE_NAME:
                ref_element = create_executable_element(token.u.name);
                cur_exec_array[cur_index] = ref_element;
                cur_index++;
                break;
            case TOKEN_LITERAL_NAME:
                ref_element = create_literal_element(token.u.name);
                cur_exec_array[cur_index] = ref_element;
                cur_index++;
                break;
            case TOKEN_SPACE:
            case TOKEN_UNKNOWN:
            default:
                break;
        }
    }while(*inout_ch != EOF);
    abort();
}

void eval_exec_array(struct ElementArray *elems) {
    int len = elems->len;
    for(int i = 0; i < elems->len; i++){
        struct Element ref_element = {ELEMENT_UNKNOWN, {0} };
        switch(elems->elements[i].etype) {
            case ELEMENT_NUMBER:
            case ELEMENT_LITERAL_NAME:
                ref_element = elems->elements[i];
                stack_push(&ref_element);
                break;
            case ELEMENT_EXECUTABLE_NAME:
                if( dict_get(elems->elements[i].u.name,&ref_element) ){
                    switch(ref_element.etype){
                        case ELEMENT_C_FUNC:
                            ref_element.u.cfunc();
                            break;
                        case ELEMENT_NUMBER:
                        case ELEMENT_LITERAL_NAME:
                            stack_push(&ref_element);
                            break;
                        case ELEMENT_EXECUTABLE_NAME:
                            eval_exec_array(ref_element.u.byte_codes);
                            break;
                        case ELEMENT_UNKNOWN:
                        default:
                            break;
                            abort();
                    }
                } else {
                    abort();
                }
                break;
            case ELEMENT_C_FUNC:
            case ELEMENT_UNKNOWN:
            default:
                break;
        }
    
    }
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
                    break;
                case TOKEN_OPEN_CURLY:
                    ref_element = compile_exec_array(&ch);
                    stack_push(&ref_element);
                    break;
                case TOKEN_CLOSE_CURLY:
                    abort();
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
                                eval_exec_array(ref_element.u.byte_codes);
                                break;
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

void mod_op(){
    struct Element num1, num2;
    stack_pop(&num1);
    stack_pop(&num2);

    struct Element mod = {ELEMENT_NUMBER, {0} };
    mod.u.number = num2.u.number % num1.u.number ;

    stack_push(&mod);
}

void eq_op(){
    struct Element bool_elem = {ELEMENT_NUMBER, {0} };
    int ref_num2 = stack_pop_int();
    int ref_num1 = stack_pop_int();

    if(ref_num1 == ref_num2){
        bool_elem.u.number = 1;
    }else{
        bool_elem.u.number = 0;
    }
    stack_push(&bool_elem);
}

void neq_op(){
    struct Element bool_elem = {ELEMENT_NUMBER, {0} };
    int ref_num2 = stack_pop_int();
    int ref_num1 = stack_pop_int();

    if(ref_num1 != ref_num2){
        bool_elem.u.number = 1;
    }else{
        bool_elem.u.number = 0;
    }
    stack_push(&bool_elem);
}

void gt_op(){
    struct Element bool_elem = {ELEMENT_NUMBER, {0} };
    int ref_num2 = stack_pop_int();
    int ref_num1 = stack_pop_int();

    if(ref_num1 > ref_num2){
        bool_elem.u.number = 1;
    }else{
        bool_elem.u.number = 0;
    }
    stack_push(&bool_elem);
}

void ge_op(){
    struct Element bool_elem = {ELEMENT_NUMBER, {0} };
    int ref_num2 = stack_pop_int();
    int ref_num1 = stack_pop_int();

    if(ref_num1 >= ref_num2){
        bool_elem.u.number = 1;
    }else{
        bool_elem.u.number = 0;
    }
    stack_push(&bool_elem);
}

void lt_op(){
    struct Element bool_elem = {ELEMENT_NUMBER, {0} };
    int ref_num2 = stack_pop_int();
    int ref_num1 = stack_pop_int();

    if(ref_num1 < ref_num2){
        bool_elem.u.number = 1;
    }else{
        bool_elem.u.number = 0;
    }
    stack_push(&bool_elem);
}

void le_op(){
    struct Element bool_elem = {ELEMENT_NUMBER, {0} };
    int ref_num2 = stack_pop_int();
    int ref_num1 = stack_pop_int();

    if(ref_num1 <= ref_num2){
        bool_elem.u.number = 1;
    }else{
        bool_elem.u.number = 0;
    }
    stack_push(&bool_elem);
}

void pop_op(){
    struct Element elem;
    stack_pop(&elem);
}

void exch_op(){
    struct Element elem1, elem2;
    stack_pop(&elem2);
    stack_pop(&elem1);
    
    stack_push(&elem2);
    stack_push(&elem1);
}

void dup_op(){
    struct Element elem;
    stack_pop(&elem);
    
    stack_push(&elem);
    stack_push(&elem);
}

void index_op(){
    int n = stack_pop_int();
    
    struct ElementArray *arr = (struct ElementArray*)malloc( sizeof(struct ElementArray) + sizeof(struct Element)*n );
    arr->len = n;
    for( int i=0; i<=n; i++ ){
        stack_pop(&arr->elements[i]);
    }
    for( int i=n; i>=0; i-- ){
        stack_push(&arr->elements[i]);
    }
    stack_push(&arr->elements[n]);
}

void roll_op(){
    int j = stack_pop_int();
    int n = stack_pop_int();
    j = j % n;
    struct ElementArray *arr = (struct ElementArray*)malloc( sizeof(struct ElementArray) + sizeof(struct Element)*n );
    arr->len = n;

    for( int i=0; i<n; i++ ){
        stack_pop(&arr->elements[i]);
    }

    for( int i=j-1; i>=0; i-- ){
        stack_push(&arr->elements[i]);
    }
    for( int i=n-1; i>=j; i-- ){
        stack_push(&arr->elements[i]);
    }
}

void exec_op(){
    struct Element proc1;
    stack_pop(&proc1);

    eval_exec_array(proc1.u.byte_codes);
}

void if_op(){
    struct Element bool1, proc1;
    stack_pop(&proc1);
    stack_pop(&bool1);

    if(1 == bool1.u.number){
        eval_exec_array(proc1.u.byte_codes);
    }
}

void ifelse_op(){
    struct Element bool1, proc1, proc2;
    stack_pop(&proc2);
    stack_pop(&proc1);
    stack_pop(&bool1);

    if(1 == bool1.u.number){
        eval_exec_array(proc1.u.byte_codes);
    }else if(0 == bool1.u.number){
        eval_exec_array(proc2.u.byte_codes);
    }else{
        abort();
    }
}

void repeat_op(){
    struct Element proc1;
    stack_pop(&proc1);
    int n = stack_pop_int();

    for(int i=0; i<n; i++){
        eval_exec_array(proc1.u.byte_codes);
    }
}

void while_op(){
    struct Element body, cond;
    stack_pop(&body);
    stack_pop(&cond);
    eval_exec_array(cond.u.byte_codes);
    int bool1 = stack_pop_int();
    while(1 == bool1){
        eval_exec_array(body.u.byte_codes);
        eval_exec_array(cond.u.byte_codes);
        bool1 = stack_pop_int();
    }
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
    register_primitive("mod", mod_op);
    register_primitive("eq", eq_op);
    register_primitive("neq", neq_op);
    register_primitive("gt", gt_op);
    register_primitive("ge", ge_op);
    register_primitive("lt", lt_op);
    register_primitive("le", le_op);
    register_primitive("pop", pop_op);
    register_primitive("exch", exch_op);
    register_primitive("dup", dup_op);
    register_primitive("index", index_op);
    register_primitive("roll", roll_op);
    register_primitive("exec", exec_op);
    register_primitive("if", if_op);
    register_primitive("ifelse", ifelse_op);
    register_primitive("repeat", repeat_op);
    register_primitive("while", while_op);
}


static void test_eval_def_and_add() {
    char *input = "/mue- 44 def /mue- 12 def /ume- 30 def ume- mue- add";
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

static void test_eval_compile_executable_array() {
    char *input = "/abc { 1 2 add } def abc";
    int expect = 1 + 2;

    cl_getc_set_src(input);
    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);

}

static void test_eval_compile_executable_array_nest() {
    char *input = "/ZZ {6} def /YY {4 ZZ 5} def /XX {1 2 YY 3} def XX mul mul mul mul add";
    int expect = 3*5*6*4*2+1;

    cl_getc_set_src(input);
    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);

}

static void test_eval_ifelse() {
    char *input = "5 1 {1 add} {2 add} ifelse 0 {1 add} {2 add} ifelse";
    int expect = 5 + 1 + 2;

    cl_getc_set_src(input);
    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_eq() {
    char *input = "/abc 42 def 42 abc eq 54 abc eq add";
    int expect = 1;

    cl_getc_set_src(input);
    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_comparison_operators() {
    char *input = "1 1 eq 2 3 neq add 3 2 gt add 5 3 ge add 2 4 lt add 3 3 le add 4 2 eq add 3 3 neq add 2 2 gt add 4 7 ge add 3 2 lt add 8 7 le add";
    int expect = 1+1+1+1+1+1+0+0+0+0+0+0;

    cl_getc_set_src(input);
    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_comparison_operators2() {
    char *input = "/hoge {1 3 3} def hoge eq add";
    int expect = 2;

    cl_getc_set_src(input);
    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_stack_operators() {
    char *input = "1 dup 2 3 dup pop exch mod add 3 4 5 6 7 4 3 roll 2 index add";
    int expect = 10;

    cl_getc_set_src(input);
    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_control_operators() {
    char *input = "{1 2 add} exec 12 {4 add} repeat";
    int expect = 1+2+4*12;

    cl_getc_set_src(input);
    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_eval_while() {
    char *input = "/hoge 1 def {16 hoge gt} {/hoge 2 hoge mul def} while hoge";
    int expect = 16;

    cl_getc_set_src(input);
    eval();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect == actual);
}

static void test_cl_getc_set_file() {
    char *input_file = "test.ps";
    int expect = 42;

    FILE *file;
    file = fopen(input_file,"r");
    cl_getc_set_file(file);
    eval();
    fclose(file);

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect = actual);
}

static void test_cl_getc_set_file_factorial() {
    char *input_file = "factorial.ps";
    int expect = 5*4*3*2*1;

    FILE *file;
    file = fopen(input_file,"r");
    cl_getc_set_file(file);
    eval();
    fclose(file);

    dict_print_all();
    stack_print_all();

    struct Element actual_element = {ELEMENT_UNKNOWN, {0} };
    stack_pop(&actual_element);
    int actual = actual_element.u.number;

    assert(expect = actual);
}

static void init_test_eval(){
    stack_clear();
    dict_clear();
    register_primitives();
}

static void unit_tests(){
    init_test_eval();
    test_eval_num_one();
    test_eval_num_two();
    test_eval_num_add();
    test_eval_num_add2();
    test_eval_def();

    init_test_eval();
    test_eval_def_and_add();
    test_eval_num_sub(); 
    test_eval_num_mul(); 
    test_eval_num_div(); 
    

    init_test_eval();
    test_eval_def_and_4_arithmetic_operators();

    init_test_eval();
    test_eval_compile_executable_array();

    init_test_eval();
    test_eval_compile_executable_array_nest();

    init_test_eval();
    test_eval_ifelse();

    init_test_eval();
    test_eval_eq();

    init_test_eval();
    test_eval_comparison_operators();
    init_test_eval();
    test_eval_comparison_operators2();

    init_test_eval();
    test_eval_stack_operators();

    init_test_eval();
    test_eval_control_operators();

    init_test_eval();
    test_eval_while();

    init_test_eval();
    test_cl_getc_set_file();

    init_test_eval();
    test_cl_getc_set_file_factorial();

}

int main() {
    unit_tests();

    return 0;
}
