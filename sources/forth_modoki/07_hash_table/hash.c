#include "clesson.h"


struct Node {
    char *key;
    int value;
    struct Node *next;
};

struct Node *array[TABLE_SIZE];

int hash(char *str) {
    unsigned int val = 0;
    while(*str) {
        val += *str++;
    }
    return (int)(val%TABLE_SIZE);
}
