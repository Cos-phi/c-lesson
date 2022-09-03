#include <stdio.h>
#include <assert.h>

static const char* const input = "123 456  1203";


int main() {
    int answer1 = 0;
    int answer2 = 0;
    int answer3 = 0;

    // write something here.
    printf("%s\n",input);
    int i = 0;
    while (1){
        switch(input[i]){
            case 32: 
                printf("space\n");
                break;
            default:
                printf("%d\n",input[i]);
        }
        if (input[i] == 0) break;
        i++;
    }

    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);
    assert(answer3 == 1203);

    return 0;
}
