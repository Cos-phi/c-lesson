#include<stdio.h>
#include<string.h>
#include<assert.h>

int str2int(char* str){
    //int num = (int)*str;
    int num = 0;
    int i = 0;
    int plusminus = 1;
    if ( '-' == str[0] ){
        i++;
        plusminus = -1;
    }
    while ( i < strlen(str) ){
        num *= 10;
        num += str[i] - '0';
        i++;
    }
    return num*plusminus;
}

void test_str2int_123(){
    char* input = "123";
    int expect = 123;

    int result = str2int(input);
    
    assert(expect == result);
}

void test_str2int_0(){
    char* input = "0";
    int expect = 0;

    int result = str2int(input);
    
    assert(expect == result);
}

void test_str2int_minus42(){
    char* input = "-42";
    int expect = -42;

    int result = str2int(input);
    
    assert(expect == result);
}

void main(){
    test_str2int_123();
    test_str2int_0();
    test_str2int_minus42();
}
