#include <stdio.h>
#include <assert.h>

static const char* const input = "123 456  1203";


int main() {
    int answer1 = 0;
    int answer2 = 0;
    int answer3 = 0;

    // write something here.
    printf("input is %s\n",input);
    int answers[3] = {0,0,0};
    int ans_n = 0;
    for (int i=0;;i++){
        if (input[i] == 0) break;
        if (input[i] == 32 && input[i-1] != 32) ans_n++;
        if (input[i] == 32) continue;
        answers[ans_n] *= 10;
        answers[ans_n] += input[i]-48;
        printf("answer%d:%d\n",ans_n+1,answers[ans_n]);
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
