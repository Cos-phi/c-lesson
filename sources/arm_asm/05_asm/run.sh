#!/usr/bin/sh
gcc *.c -o asm.out
./asm.o test/test_input/$1.ks asm_tmp_$1.bin
cmp asm_tmp_$1.bin test/test_expect/$1.bin
rm asm_tmp_$1.bin
rm asm.out

