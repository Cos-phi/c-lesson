#include <stdio.h>
#include <assert.h>
#include <string.h>

#define BUF_SIZE 64

static char buf[BUF_SIZE];
static int pos;
static const char* input = "123\n4567\n89ABC\n";

int cl_getline(char **out_buf){
    int len = 0;
    while('\n' != input[pos]){
        buf[len++] = input[pos++]; // bufにつめる
    }
    pos++;
    buf[len] = '\0';

    *out_buf = buf;
    return len;
}

void cl_getline_set_src(char* str){
    input = str;
    pos = 0;
}

/**
static void test_cl_getline(){
    //関数の外で、次のように宣言して代入済  static const char* input = "123\n4567\n89ABC\n";
    char* expect_str1 = "123";
    char* expect_str2 = "4567";
    char* expect_str3 = "89ABC";
    int expect_len1 = 3;
    int expect_len2 = 4;
    int expect_len3 = 5;
     
    char* actual_str1;
    char* actual_str2;
    char* actual_str3;

    int actual_len1 = cl_getline(&actual_str1);
    assert(expect_len1 == actual_len1);
    assert(0 == strcmp(expect_str1, actual_str1));
    
    int actual_len2 = cl_getline(&actual_str2);
    assert(expect_len2 == actual_len2);
    assert(0 == strcmp(expect_str2, actual_str2));

    int actual_len3 = cl_getline(&actual_str3);
    assert(expect_len3 == actual_len3);
    assert(0 == strcmp(expect_str3, actual_str3));
}

static void test_cl_getline_set_src(){
    char* input = "abc\n1234\n";
    char* expect_str1 = "abc";
    char* expect_str2 = "1234";
    int expect_len1 = 3;
    int expect_len2 = 4;
     
    char* actual_str1;
    char* actual_str2;

    cl_getline_set_src(input);
    int actual_len1 = cl_getline(&actual_str1);
    assert(expect_len1 == actual_len1);
    assert(0 == strcmp(expect_str1, actual_str1));
    
    int actual_len2 = cl_getline(&actual_str2);
    assert(expect_len2 == actual_len2);
    assert(0 == strcmp(expect_str2, actual_str2));
}

static void unit_tests(){
    test_cl_getline();
    test_cl_getline_set_src();
}
int main(){
    unit_tests();
}
**/
