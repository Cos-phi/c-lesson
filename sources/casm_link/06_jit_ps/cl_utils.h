#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

void cl_clear_output();
char *cl_get_result(int num);
char *cl_get_all_result();
void cl_enable_buffer_mode();
void cl_disable_buffer_mode();
void cl_printf(char *fmt, ...);
