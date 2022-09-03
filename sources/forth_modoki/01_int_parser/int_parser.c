#include <stdio.h>
#include <assert.h>

static const char* const input = "123 456  1203";


int main() {
    int answer1 = 0;
    int answer2 = 0;
    int answer3 = 0;

    // write something here.
    printf("%s\n",input);
    int answers[3] = {0,0,0};
    int j = 0;
    for (int i=0;;i++){
        if (input[i] == 0) break;
        if (input[i] == 32 && input[i-1] != 32) {
            j++;
            printf("space!\n");
        }
        if (input[i] == 32) continue;
        answers[j] *= 10;
        answers[j] += input[i]-48; 
        printf("%d:%d\n",j,answers[j]);
    }
    answer1 = answers[0];
    answer2 = answers[1];
    answer3 = answers[2];

    // verity result.
    assert(answer1 == 123);
    assert(answer2 == 456);
    assert(answer3 == 1203);

    return 0;
}
