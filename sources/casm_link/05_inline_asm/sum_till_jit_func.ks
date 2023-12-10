    mov r1, #0
    mov r2, #0
loop:
    cmp r1, r0
    blt end
    add r2, r2, r1
    add r1, r1, #1
    b loop
end:
    mov r0, #42
    mov r15, r14
