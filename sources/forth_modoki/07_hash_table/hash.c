#include "clesson.h"

#define TABLE_SIZE 1024

struct Node {
    char *key;
    int value;
    struct Node *next;
};

struct Node *array[1024];

int hash(char *str) {
    unsigned int val = 0;
    while(*str) {
        val += *str++;
    }
    return (int)(val%1024);
}
