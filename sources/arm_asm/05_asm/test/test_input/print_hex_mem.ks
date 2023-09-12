  ldr r13,=0x08000000
  mov r0, r15
  bl print_hex
  mov r0, #0x68
  bl print_hex
  mov r0, r15
  bl print_hex
  bl tekitou_label
end:
  b end
print_hex:
  stmdb r13!,{r0,r1,r3,r14}
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
  bl putchar
  sub r3,r3,#4
  cmp r3,#0
  bge _loop
  mov r0,#0x0D
  bl putchar
  mov r0,#0x0A
  bl putchar
  ldmia r13!,{r0,r1,r3,r14}
  mov r15,r14
putchar:
  stmdb r13!,{r1,r14}
  ldr r1,[r15,#20]
  str r0, [r1]
  ldmia r13!,{r1,r14}
  mov r15,r14
tekitou_label:
  mov r0, r14
  bl print_hex
  mov r15,r14
.raw 0x101f1000