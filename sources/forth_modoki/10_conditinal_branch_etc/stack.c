#include "element.h"
#include "clesson.h"

#define STACK_SIZE 1024

static struct Element stack[STACK_SIZE];
static int stack_index = 0;

void stack_push(struct Element *input_element) {
    stack_index++;
    stack[stack_index].etype = input_element->etype;
    switch(input_element->etype){
        case ELEMENT_NUMBER:
            stack[stack_index].u.number = input_element->u.number;
            break;
        case ELEMENT_LITERAL_NAME:
            stack[stack_index].u.name = input_element->u.name;
            break;
        case ELEMENT_EXECUTABLE_NAME:
            stack[stack_index].u.byte_codes = input_element->u.byte_codes;
            break;
        case ELEMENT_UNKNOWN:
            break;
    }
}

void stack_pop(struct Element *out_element) {
    if(0 == stack_index){
        struct Element out_element = {ELEMENT_UNKNOWN, {0}};
        return;
    }
    out_element->etype = stack[stack_index].etype ;
    switch(stack[stack_index].etype){
        case ELEMENT_NUMBER:
            out_element->u.number = stack[stack_index].u.number;
            break;
        case ELEMENT_LITERAL_NAME:
            out_element->u.name = stack[stack_index].u.name;
            break;
        case ELEMENT_EXECUTABLE_NAME:
            out_element->u.byte_codes = stack[stack_index].u.byte_codes;
            break;
        case ELEMENT_UNKNOWN:
            break;
    }
    stack_index--;
}

int stack_pop_int(){
    int out_num;

    struct Element elem;
    stack_pop(&elem);

    struct Element ref_elem = {ELEMENT_UNKNOWN, {0} };
    switch(elem.etype){
        case ELEMENT_NUMBER:
            out_num = elem.u.number;
            break;
        case ELEMENT_EXECUTABLE_NAME:
            if( dict_get(elem.u.name,&ref_elem) ){
                switch(ref_elem.etype){
                    case ELEMENT_NUMBER:
                        out_num = elem.u.number;
                        break;
                    case ELEMENT_C_FUNC:
                    case ELEMENT_LITERAL_NAME:
                    case ELEMENT_EXECUTABLE_NAME:
                    case ELEMENT_UNKNOWN:
                        abort();    
                        break;
                }
            } 
            break;
        case ELEMENT_LITERAL_NAME:
        case ELEMENT_C_FUNC:
        case ELEMENT_UNKNOWN:
            abort();
    }

    return out_num;
}

void stack_clear(){
    stack_index = 0;
}

void stack_print_all(){
    printf("stack(cur index is %d)\n",stack_index);
    for( int i = 1; i <= stack_index; i++){
        printf(" type%d:",stack[i].etype); 
        switch(stack[i].etype){
            case ELEMENT_NUMBER:
                printf("num %d \n",stack[i].u.number);
                break;
            case ELEMENT_LITERAL_NAME:
                printf("literal %s \n",stack[i].u.name);
                break;
            case ELEMENT_EXECUTABLE_NAME:
                printf("[%d] \n",stack[i].u.byte_codes->len);
                break;
            case ELEMENT_UNKNOWN:
                break;
        }
    }
    printf("\n");
}


static int isequal_element(struct Element *element1, struct Element *element2) {
    if (element1->etype != element2->etype){
        return 0;
    }else{
        switch(element1->etype) {
            case ELEMENT_NUMBER:
                if (element1->u.number == element2->u.number) return 1;
                else return 0;
            case ELEMENT_LITERAL_NAME:
                if (0 == strcmp(element1->u.name, element2->u.name)) return 1;
                else return 0;
            default:
                return 1;
        }
    }
}

static void test_isequal_elements_are_equal(){
    struct Element input_1;
    input_1.etype = ELEMENT_LITERAL_NAME;
    input_1.u.name = "abc";
    struct Element input_2;
    input_2.etype = ELEMENT_LITERAL_NAME;
    input_2.u.name = "abc";
    int expect = 1;

    int result = isequal_element(&input_1,&input_2);
    
    assert (expect == result);
}

static void test_one_pop(){
    struct Element expect = {ELEMENT_UNKNOWN, {0}};
    struct Element output = {ELEMENT_UNKNOWN, {0}};

    stack_clear();
    stack_pop(&output);

    assert (1 == isequal_element(&expect,&output));
}

static void test_one_push(){
    struct Element input; input.etype = ELEMENT_NUMBER; input.u.number = 42;
    struct Element expect; expect.etype = ELEMENT_NUMBER; expect.u.number = 42;

    stack_clear();
    stack_push(&input);
    
    assert (1 == isequal_element(&expect,&stack[stack_index]));
}

static void test_one_push_one_pop(){
    struct Element input;  input.etype  = ELEMENT_NUMBER; input.u.number  = 33147;
    struct Element expect; expect.etype = ELEMENT_NUMBER; expect.u.number = 33147;
    struct Element output;

    stack_clear();
    stack_push(&input);
    stack_pop(&output);

    assert (1 == isequal_element(&expect,&output));
}
static void test_two_push_two_pop(){
    struct Element input1;  input1.etype  = ELEMENT_NUMBER;       input1.u.number  = -42;
    struct Element input2;  input2.etype  = ELEMENT_LITERAL_NAME; input2.u.name    = "paooonf";
    struct Element expect1; expect1.etype = ELEMENT_LITERAL_NAME; expect1.u.name   = "paooonf";
    struct Element expect2; expect2.etype = ELEMENT_NUMBER;       expect2.u.number = -42;
    struct Element output1;
    struct Element output2;

    stack_clear();
    stack_push(&input1);
    stack_push(&input2);
    stack_pop(&output1);
    stack_pop(&output2);

    assert (1 == isequal_element(&expect1,&output1));
    assert (1 == isequal_element(&expect2,&output2));

}

static void unit_tests() {
    test_isequal_elements_are_equal();
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
