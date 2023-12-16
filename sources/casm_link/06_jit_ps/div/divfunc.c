#include <stdio.h>
#include <sys/mman.h>
#include <assert.h>

int div(int r2, int r3){
    int ansp = 0; //r4 (anser positve)
    int ansn = 0; //r5 (anser negative)
    while(1){
        ansp++;
        if( (r2-r3*ansp)*(r2-r3*ansp) < r3*r3 ) { // r8(=r7*r7) < r6
            return ansp;
        }
        
        ansn--;
        if( (r2-r3*ansn)*(r2-r3*ansn) < r3*r3 ){ // r8(=r7*r7) < r6
            return ansn;
        }
    }
}

int div_inline(int a, int b) {
    int res;

    asm("mov r2, r0");
    asm("mov r3, r1");
    asm("mov r4, #0"); // int ansp = 0
    asm("mov r5, #0"); // int ansn = 0
    asm("mul r6, r3, r3"); // r6 = r3*r3   
    asm("loop:");
    asm("add r4, r4, #1"); // ansp++    
    asm("sub r5, r5, #1"); // ansn--
    
    asm("mul r7, r4, r3"); //       r3*ansp
    asm("sub r7, r2, r7"); //       r2-r3*ansp 
    asm("mul r8, r7, r7"); // r8 = (r2-r3*ansp)*(r2-r3*ansp)
    asm("cmp r8, r6");
    asm("blt end_ansp");
     
    asm("mul r7, r5, r3"); //       r3*ansn
    asm("sub r7, r2, r7"); //       r2-r3*ansn
    asm("mul r8, r7, r7"); // r8 = (r2-r3*ansn)*(r2-r3*ansn)
    asm("cmp r8, r6");
    asm("blt end_ansn");
        
    asm("b loop");

    asm("end_ansp:");
    asm("mov r9,r4");
    asm("b end");

    asm("end_ansn:");
    asm("mov r9,r5");
    asm("b end");

    asm("end:");
    asm("mov %0, r9" :"=r"(res));
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

void main(){
    assert( 6 == div(42, 7));
    assert(-6 == div(42,-7));
    assert(-6 == div(-42,7));
    assert(12 == div(-49,-4));

    assert( 6 == div_inline(44, 7));
    assert(-6 == div_inline(42,-7));
    assert(-6 == div_inline(-42,7));
    assert(12 == div_inline(-49,-4));

    int res;
    int (*funcvar)(int);
    binary_buf = allocate_executable_buf(1024);

    jit_sum_till();

    funcvar = (int(*)(int))binary_buf;
    res = funcvar(10);
    assert(res == 45);
}