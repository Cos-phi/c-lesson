.globl _start
_start:

//  mov r0, r15
  ldr r0, =0xdeadbeaf
  bl print_hex

  mov r0, #0x68
  bl print_hex

end:
  b end

/* 
  print_hex
   arg r0: taget register
   print r0 in the form of hex on UART 
   r0 is current charactor
   r1 is target register (arg r0)
   r3 is counter 
*/
print_hex:
  stmdb r13!,{r0,r1,r3}
  ldr r4,=0x101f1000
  mov r1,r0
  mov r3,#28 
_loop:
  lsr r0,r1,r3
  and r0,r0,#0x0F
  cmp r0,#10
  add r0,r0,#48
  blt _print
  add r0,r0,#7
_print:
//  bl putchar
  str r0,[r4]
  sub r3,#4
  cmp r3,#0
  bge _loop
  mov r0,#0x0D
//  bl putchar
  str r0,[r4]
  mov r0,#0x0A
  str r0,[r4]
//  bl putchar
  ldmia r13!,{r0,r1,r3}
  mov r15,r14

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
  mov r15,r14

