#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

//dict.c
void dict_unittests();
void dict_put(int key, int value);
int dict_get(int key, int* out_value);
void dict_clear();