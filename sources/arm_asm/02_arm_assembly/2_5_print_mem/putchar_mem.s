/*
  r13: stack pointer, which points stack end.
  r14: program counter num which is evacuated templorary when function called.
  r15: program counter
*/

.globl _start
_start:

  ldr r13,=0x08000000
  ldr r0,=msg1
  bl print
  ldr r0,=msg2
  bl print
end:
  b end

/*
  putchar:
   arg r0: target character which is printed. 
   used internal register: r1.
*/
putchar:
  stmdb r13!,{r1,r14}
  ldr r1,=0x101f1000
  str r0, [r1]
  ldmia r13!,{r1,r14}
  mov r15, r14

/*
  print:
    arg r0: address of target string, must end by \0.
    used internal register: r0, r1, r3.
*/
print:
  stmdb r13!,{r0,r1,r3,r14}
  ldrb r3,[r0] // load one char
_loop:  

  stmdb r13!,{r0}
  mov r0,r3
  bl putchar
  ldmia r13!,{r0}

  add r0, r0, #1
  ldrb r3,[r0] // load one char
  cmp r3,#0  // if the char is \0 ..
  bne _loop
  ldmia r13!,{r0,r1,r3,r14}
  mov r15, r14

msg1: .asciz "First text.\n"
msg2: .asciz "Second text!\n"
