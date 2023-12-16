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
    asm("loop:");
    asm("add r4, r4, #1"); // ansp++    
    asm("sub r5, r5, #1"); // ansn--
    asm("mul r6, r3, r3"); // r6 = r3*r3   
    
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


void main(){
    assert( 6 == div(42, 7));
    assert(-6 == div(42,-7));
    assert(-6 == div(-42,7));
    assert(12 == div(-49,-4));

    assert( 6 == div_inline(44, 7));
    assert(-6 == div_inline(42,-7));
    assert(-6 == div_inline(-42,7));
    assert(12 == div_inline(-49,-4));
}