#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<math.h>

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


void test_int2str_123(){
    int input = 123;
    char* expect = "123";

    int str_size = (int)(sizeof(int)*log10(256))+1+1; // size*byte + "\n" + '-'
    char out_str[str_size];
    int2str(input,out_str);
    
    assert(0 == strcmp(expect, out_str));
}

void test_int2str_0(){
    int input = 0;
    char* expect = "0";

    int str_size = (int)(sizeof(int)*log10(256))+1+1; // size*byte + "\n" + '-'
    char out_str[str_size];
    int2str(input,out_str);
    
    assert(0 == strcmp(expect, out_str));
}

void test_int2str_minus42(){
    int input = -42;
    char* expect = "-42";

    int str_size = (int)(sizeof(int)*log10(256))+1+1; // size*byte + "\n" + '-'
    char out_str[str_size];
    int2str(input,out_str);
    
    assert(0 == strcmp(expect, out_str));
}

void main(){
    test_int2str_123();
    test_int2str_0();
    test_int2str_minus42();
}
