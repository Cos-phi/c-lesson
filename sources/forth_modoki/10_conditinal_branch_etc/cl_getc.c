#include <stdio.h>
#include <string.h>

static const char* input = "123 456";
static int pos = 0;
static FILE *file = NULL;
static int cur_ch;


int cl_getc(){
    if(file == NULL){
        if(strlen(input) == pos){
            return EOF;
        }else{
            return input[pos++];
        }
    }else{
        cur_ch = fgetc(file);
        return cur_ch;
    }
}

void cl_getc_set_src(char* str){
    input = str;
    pos = 0;
}

void cl_getc_set_file(FILE* input_file){
    file = input_file;
}
