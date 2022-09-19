#include "stack.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

int str2int(char* str){
    //int num = (int)*str;
    int num = 0;
    int i = 0;
    int sign = 1;
    if ( '-' == str[0] ){
        i++;
        sign = -1;
    }
    while ( i < strlen(str) ){
        num *= 10;
        num += str[i] - '0';
        i++;
    }
    return num*sign;
}

void int2str(int num, char* out_str){
    int cur_pos = 0;
    if ( 0 == num ){
        out_str[cur_pos++] = '0';
    }else if ( 0 > num ){
        out_str[cur_pos] = '-';
        num = (-1)*num;
        cur_pos++;
    }
    while (0 != num){
        int num_power = (int)(log10(num));
        int cur_num = (int)(num/pow(10,num_power));
        out_str[cur_pos] = '0' + cur_num;
        num -= (int)(num / pow(10,num_power)) * pow(10,num_power);
        cur_pos++;
    }
    out_str[cur_pos] = '\0';
}


static void test_str2int_123(){
    char* input = "123";
    int expect = 123;

    int result = str2int(input);
    
    assert(expect == result);
}

static void test_str2int_0(){
    char* input = "0";
    int expect = 0;

    int result = str2int(input);
    
    assert(expect == result);
}

static void test_str2int_minus42(){
    char* input = "-42";
    int expect = -42;

    int result = str2int(input);
    
    assert(expect == result);
}

static void test_int2str_123(){
    int input = 123;
    char* expect = "123";

    int str_size = (int)(sizeof(int)*log10(256))+1+1; // size*byte + "\n" + '-'
    char out_str[str_size];
    int2str(input,out_str);
    
    assert(0 == strcmp(expect, out_str));
}

static void test_int2str_0(){
    int input = 0;
    char* expect = "0";

    int str_size = (int)(sizeof(int)*log10(256))+1+1; // size*byte + "\n" + '-'
    char out_str[str_size];
    int2str(input,out_str);
    
    assert(0 == strcmp(expect, out_str));
}

static void test_int2str_minus42(){
    int input = -42;
    char* expect = "-42";

    int str_size = (int)(sizeof(int)*log10(256))+1+1; // size*byte + "\n" + '-'
    char out_str[str_size];
    int2str(input,out_str);
    
    assert(0 == strcmp(expect, out_str));
}

static void unit_tests_intstr(){
    test_int2str_123();
    test_int2str_0();
    test_int2str_minus42();
    test_str2int_123();
    test_str2int_0();
    test_str2int_minus42();
}
/**
void main(){
    unit_tests_intstr();
}
**/
