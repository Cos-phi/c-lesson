#include <stdio.h>
#include <sys/mman.h>

int sum_till(int a) {
    int sum=0;
    for(int i = 0; i < a; i++) {
        sum+=i;
    }
    return sum;
}

int sum_till_inline(int a) {
    int res;
    /*
    Use r1 as loop counter.
    Use r2 to store temp result.
    */
    asm("mov r1, #0");
    asm("mov r2, #0");
    asm("loop:");
    asm("cmp r0, r1");
    asm("beq end");
    asm("add r2, r2, r1"); // sum+=i
    asm("add r1, r1, #1"); // i++    
    asm("b loop");
    asm("end:");
    asm("mov %0, r2" :"=r"(res));
    return res;
}


/*
JIT
*/
int *binary_buf;

int* allocate_executable_buf(int size) {
    return (int*)mmap(0, size,
                 PROT_READ | PROT_WRITE | PROT_EXEC,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}


void jit_sum_till() {
    /* TODO:
    Place binary to binary_buf here.
    Hint: put almost the same binary as sum_till_inline.
    Compile and use arm-linux-gnueabi-objdump -S ./a.out to check binary.

    */
    // dummy implementation 
    
    binary_buf[0] = 0xe3a01000; //0x00 mov	r1, #0
    binary_buf[1] = 0xe3a02000; //0x04 mov	r2, #0
    binary_buf[2] = 0xe1500001; //0x08 cmp	r0, r1 (label loop:)
    binary_buf[3] = 0xda000002; //0x0c blt	0x1c (b end)
    binary_buf[4] = 0xe0822001; //0x10 add	r2, r2, r1
    binary_buf[5] = 0xe2811001; //0x14 add	r1, r1, #1
    binary_buf[6] = 0xeafffffa; //0x18 b	0x8 (b loop)
    binary_buf[7] = 0xe1a00002; //0x1c mov	r0, r2 (label end:)
    binary_buf[8] = 0xe1a0f00e; //0x20 mov	pc, lr (mov r15, r14)
}


/*
test code
*/


void assert_true(int boolflag) {
    if(!boolflag) {
        printf("assert fail\n");
    }
}


int main() {
    int res;
    int (*funcvar)(int);

    res = sum_till(10);
    assert_true(res == 45);

    res = sum_till_inline(10);
    assert_true(res == 45);

    /* TODO: Pass below code.
    */
    binary_buf = allocate_executable_buf(1024);

    jit_sum_till();

    funcvar = (int(*)(int))binary_buf;
    res = funcvar(10);
    assert_true(res == 45);

    res = funcvar(11);
    assert_true(res == 55);

    return 0;
}

