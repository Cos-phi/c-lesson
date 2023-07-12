#!/usr/bin/sh
gcc *.c -o asm.out
./asm.o test/test_input/$1.ks > test/asm_tmp_$1.bin
cmp test/asm_tmp_$1.txt test/test_expect/$1.txt 
rm test/asm_tmp_$1.txt
rm asm.out:x

