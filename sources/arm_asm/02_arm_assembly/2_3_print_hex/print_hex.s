/*
 arm-none-eabi-as print_hex.s -o print_hex.o
 arm-none-eabi-ld print_hex.o -Ttext 0x00010000 -o print_hex.elf
 arm-none-eabi-objcopy print_hex.elf -O binary print_hex.bin
 qemu-system-arm -M versatilepb -m 128M -nographic -kernel print_hex.bin -serial mon:stdio
*/
.globl _start
_start:
    ldr r1,=0xdeadbeaf
    b print_hex
print_hex:
    // TODO: implement here
    ldr r0,=0x101f1000
    lsr r2,r1,#28
    and r2,r2,#0x0F
    add r2,r2,#55
    str r2,[r0]
    mov r4,#0x0D
    str r4,[r0]
    mov r4,#0x0A
    str r4,[r0]
end:
    b end
