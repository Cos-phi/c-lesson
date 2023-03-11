#!/usr/bin/sh
gcc *.c -o disasm.o
./disasm.o test/test_input/$1.bin > disasm_tmp_$1.txt
diff disasm_tmp_$1.txt test/test_expect/$1.txt 
rm disasm_tmp_$1.txt
rm disasm.o
