#include "element.h"
#include "clesson.h"

int hash(char *str) {
    unsigned int val = 0;
    while(*str) {
        val += *str++;
    }
    int table_size = TABLE_SIZE;
    int hashval = (int)val % table_size;
    return hashval;
}
