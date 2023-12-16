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

void jit_div() {
    binary_buf[ 1] = 0xe1a02000; // 	mov	r2, r0
    binary_buf[ 2] = 0xe1a03001; // 	mov	r3, r1
    binary_buf[ 3] = 0xe3a04000; // 	mov	r4, #0
    binary_buf[ 4] = 0xe3a05000; // 	mov	r5, #0
    binary_buf[ 5] = 0xe0060393; // 	mul	r6, r3, r3
    binary_buf[ 6] = 0xe2844001; // 	add	r4, r4, #1
    binary_buf[ 7] = 0xe2455001; // 	sub	r5, r5, #1
    binary_buf[ 8] = 0xe0070394; // 	mul	r7, r4, r3
    binary_buf[ 9] = 0xe0427007; // 	sub	r7, r2, r7
    binary_buf[10] = 0xe0080797; // 	mul	r8, r7, r7
    binary_buf[11] = 0xe1580006; // 	cmp	r8, r6
    binary_buf[12] = 0xba000005; // 	blt	0x9b4
    binary_buf[13] = 0xe0070395; // 	mul	r7, r5, r3
    binary_buf[14] = 0xe0427007; // 	sub	r7, r2, r7
    binary_buf[15] = 0xe0080797; // 	mul	r8, r7, r7
    binary_buf[16] = 0xe1580006; // 	cmp	r8, r6
    binary_buf[17] = 0xba000002; // 	blt	0x9bc
    binary_buf[18] = 0xeafffff2; // 	b	0x980
    binary_buf[19] = 0xe1a09004; // 	mov	r9, r4
    binary_buf[20] = 0xea000001; // 	b	0x9c4
    binary_buf[21] = 0xe1a09005; // 	mov	r9, r5
    binary_buf[22] = 0xeaffffff; // 	b	0x9c4
    binary_buf[23] = 0xe1a00009; // 	mov	r0, r9
    binary_buf[24] = 0xe1a0f00e; //     mov	pc, lr (mov r15, r14)5678910

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

    int (*funcvar)(int,int);
    binary_buf = allocate_executable_buf(1024);
    jit_div();
    funcvar = (int(*)(int,int))binary_buf;

    assert( 6 == funcvar(44, 7));
    assert(-6 == funcvar(42,-7));
    assert(-6 == funcvar(-42,7));
    assert(12 == funcvar(-49,-4));
}