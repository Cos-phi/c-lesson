.globl _start
_start:

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
  ldr r1,=0x101f1000
  str r2, [r1]
  mov r15, r14


/*
  print:
    arg r0: Address of target string, must end by \0.
    used internal register: r0, r1, r3.
*/
print:
  ldrb r3,[r0] // load one char
_loop:
  // TODO: use putchar here someway.
  /*
  str r3,[r0]
  */
  mov r2,r3
  mov r4,r14
  bl putchar
  mov r14,r4

  add r0, r0, #1
  ldrb r3,[r0]
  cmp r3,#0
  bne _loop
  mov r15, r14

msg1: .asciz "First text.\n"
msg2: .asciz "Second text!\n"
