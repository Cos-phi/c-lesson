#include "element.h"
#include "clesson.h"

int hash(char *str) {
    unsigned int val = 0;
    while(*str) {
        val += *str++;
    }
    return (int)(val%TABLE_SIZE);
}
