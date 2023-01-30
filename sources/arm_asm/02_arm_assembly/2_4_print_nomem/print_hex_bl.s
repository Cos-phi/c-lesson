.globl _start
_start:

  mov r0, r15
  bl print_hex

  mov r0, #0x68
  bl print_hex

end:
  b end


print_hex:
/* print r0 in the form of hex on UART */
/* r1 is UART address, r2 is the current charactor */
/* r3 is counter */
  ldr r1,=0x101f1000
  mov r3,#28 
loop:
  lsr r2,r0,r3
  and r2,r2,#0x0F
  cmp r2,#10
  add r2,r2,#48
  blt print
  add r2,r2,#7
print:
  str r2,[r1]
  sub r3,#4
  cmp r3,#0
  bge loop
  mov r4,#0x0D
  str r4,[r1]
  mov r4,#0x0A
  str r4,[r1]
  mov r15,r14

