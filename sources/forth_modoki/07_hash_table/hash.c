#include "clesson.h"


struct Node {
    char *key;
    int value;
    struct Node *next;
};

int hash(char *str) {
    unsigned int val = 0;
    while(*str) {
        val += *str++;
    }
    return (int)(val%TABLE_SIZE);
}
