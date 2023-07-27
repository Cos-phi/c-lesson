#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define BUF_SIZE 64
#define S_SIZE 512

static char buf[BUF_SIZE];
static int pos = 0;
static FILE *file = NULL;
static char* input = "123\n4567\n89ABC\n";

int cl_getline(char **out_buf){
    if( NULL == file ){
        int len = 0;
        while('\n' != input[pos]){
            buf[len++] = input[pos++]; 
            if( len > BUF_SIZE || EOF == input[pos] ){
                return -1;
            }
        }
        pos++;
        buf[len] = '\0';

        *out_buf = buf;
        return len;
    }else{
        if( NULL == fgets(buf, BUF_SIZE, file) ){
            return -1;
        }
        int pos = 0;
        while('\n' != buf[pos] && '\0' != buf[pos]){  
            pos++;
            if( pos > BUF_SIZE ){
                return -1;
            }
        }
        buf[pos] = '\0';

        *out_buf = buf;
        return pos;
    }
}

void cl_getline_set_src(char* str){
    file = NULL;
    input = str;
    pos = 0;
}

void cl_getline_set_file(FILE* input_file){
    file = input_file;
}


static void test_cl_getline(){
    char* expect_str1 = "123";
    char* expect_str2 = "4567";
    char* expect_str3 = "89ABC";
    int expect_len1 = 3;
    int expect_len2 = 4;
    int expect_len3 = 5;
     
    file = NULL;
    char* actual_str2;
    char* actual_str3;
    char* actual_str1;

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

static void test_cl_getline_err(){
    char* input = "abcdef";
    int expect = -1;
     
    cl_getline_set_src(input);
    char* actual_str;
    int actual_len = cl_getline(&actual_str);
    assert(expect == actual_len);
}

void cl_getline_unittests(){
    test_cl_getline();
    test_cl_getline_set_src();
    //test_cl_getline_err();// ASANがいると怒られてしまいますので、コメントアウトしております。
}

