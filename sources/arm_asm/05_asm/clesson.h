#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef STRUCT_H
#define STRUCT_H

#define PARSE_FAIL -1

struct Substring {
    char *str;
    int len;
};

#endif

//cl_getline.c
void cl_getline_unittests();
int cl_getline(char **out_buf);
void cl_getline_set_src(char* str);
void cl_getline_set_file(FILE* input_file);

//cl_binarytree.c
void cl_binarytree_unittests();
int to_mnemonic_symbol(char *str, int len);
int to_label_symbol(char *str, int len);
void init_mnemonic_tree();
void init_label_tree();
int substr_to_mnemonic_symbol(struct Substring substr);
int substr_to_label_symbol(struct Substring substr);
 
//dict.c
void dict_unittests();
void dict_emitter_pos_put(int label_symbol, int address);
int dict_emitter_pos_get(int label_symbol);
void dict_clear();

//parser.c
void parser_unittests();
int substreq(char* s1, struct Substring s2);
int streq(char* s1, char* s2);
int parse_immediate_value(char* str, int* out_value);
int parse_register(char* str, int* out_register);
int parse_one(char *str, struct Substring* out_subs);
int parse_raw_value(char* str, int* out_value);
int parse_string(char* input, char **out_str);
int skip_whitespace(char* str);
int skip_comma(char* str);
int skip_sbracket(char* str);
int skip_cbracket(char* str);
int skip_equal(char* str);
int skip_exclamation_mark(char* str);
int is_register(char* str);
int is_sbracket(char* str);
int is_cbracket(char* str);
int is_doublequotation(char* str);
int is_equal(char* str);
int is_exclamation_mark(char* str);