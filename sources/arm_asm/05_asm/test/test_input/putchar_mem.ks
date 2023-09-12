  ldr r13,=0x08000000
  ldr r0,=msg1
  bl print
  ldr r0,=msg2
  bl print
end:
  b end
putchar:
  stmdb r13!,{r1,r14}
  ldr r1,[r15,#0x30]
  str r0, [r1]
  ldmia r13!,{r1,r14}
  mov r15, r14
print:
  stmdb r13!,{r0,r1,r3,r14}
  mov r1,r0
  ldrb r0,[r1]
_loop:  
  bl putchar
  add r1, r1, #1
  ldrb r0,[r1]
  cmp r0,#0
  bne _loop
  ldmia r13!,{r0,r1,r3,r14}
  mov r15, r14
.raw 0x101f1000
msg1: 
  .raw "First text.\n"
msg2:
  .raw "Second text!\n"

