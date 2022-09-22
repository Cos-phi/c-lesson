#include "stack.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

int str2int(char* str){
    //int num = ( (int)str[0] + (int)(str[1]<<8) + (int)(str[2]<<16) + (int)(str[3]<<24) );
    int num = 0;
    if ( 0x80 > str[0] ){
        num += (int) str[0]        & 0x000000ff;
    }else{
        num += (int) str[0]        & 0xffffffff;
    }
    if ( 0x80 > str[1] ){
        num += (int) str[1] <<  8  & 0x0000ff00;
    }else{
        num += (int) str[1] <<  8  & 0xffffff00;
    }
    if ( 0x80 > str[2] ){
        num += (int) str[2] << 16  & 0x00ff0000;
    }else{
        num += (int) str[2] << 16  & 0xffff0000;
    }
    if ( 0x80 > str[3] ){
        num += (int) str[3] << 24  & 0xff000000;
    }else{
        num += (int) str[3] << 24  & 0xff000000;
    }
        
    return num;
}

void int2str(int num, char* out_str){
    out_str[0] = (char)( num & 0x000000ff  );
    out_str[1] = (char)((num & 0x0000ff00) >>  8);
    out_str[2] = (char)((num & 0x00ff0000) >> 16);
    out_str[3] = (char)((num & 0xff000000) >> 24);
    out_str[4] = '\0';
}

static void test_str2int_33147(){
    char input[4];
    input[0] = 0x7b;
    input[1] = 0x81;
    input[2] = 0x00;
    input[3] = 0x00;
    int expect = 33147;

    int result = str2int(input);
    
    assert(expect == result);
}

static void test_str2int_123(){
    char input[4];
    input[0] = 0x7b;
    input[1] = 0x00;
    input[2] = 0x00;
    input[3] = 0x00;
    int expect = 123;

    int result = str2int(input);
    
    assert(expect == result);
}

static void test_str2int_0(){
    char input[4];
    input[0] = 0x00;
    input[1] = 0x00;
    input[2] = 0x00;
    input[3] = 0x00;
    int expect = 0;

    int result = str2int(input);
    
    assert(expect == result);
}

static void test_str2int_minus42(){
    char input[4];
    input[0] = 0xD6;
    input[1] = 0xFF;
    input[2] = 0xFF;
    input[3] = 0xFF;
    int expect = -42;

    int result = str2int(input);
    
    assert(expect == result);
}

static void test_int2str_123(){
    int input = 123;
    char expect[4];
    expect[0] = 0x7b;
    expect[1] = 0x00;
    expect[2] = 0x00;
    expect[3] = 0x00;
    expect[4] = '\0';

    int str_size = (int)(sizeof(int)*log10(256))+1+1; // size*byte + "\n" + '-'
    char out_str[str_size];
    int2str(input,out_str);
    
    assert(0 == strcmp(expect, out_str));
}

static void test_int2str_0(){
    int input = 0;
    char expect[5];
    expect[0] = 0x00;
    expect[1] = 0x00;
    expect[2] = 0x00;
    expect[3] = 0x00;
    expect[4] = '\0';

    int str_size = (int)(sizeof(int)*log10(256))+1+1; // size*byte + "\n" + '-'
    char out_str[str_size];
    int2str(input,out_str);
    
    assert(0 == strcmp(expect, out_str));
}

static void test_int2str_minus42(){
    int input = -42;
    char expect[5];
    expect[0] = 0xD6;
    expect[1] = 0xFF;
    expect[2] = 0xFF;
    expect[3] = 0xFF;
    expect[4] = '\0';

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
    test_str2int_33147();
}
/*

void main(){
    unit_tests_intstr();
}

*/
