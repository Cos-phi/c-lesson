#!/usr/bin/env perl
my $asmfile = $ARGV[0];
$asmfile =~ s/^(.*)\..*$/$1/;
print "終了はC-a xです。\n";
print "別のターミナルでarm-none-eabi-gdbを実行してください。\n\n";
system "arm-none-eabi-as $asmfile.s -o $asmfile.o";
system "arm-none-eabi-ld $asmfile.o -Ttext 0x00010000 -o $asmfile.elf";
system "arm-none-eabi-objcopy $asmfile.elf -O binary $asmfile.bin";
system "qemu-system-arm -M versatilepb -m 128M -nographic -kernel $asmfile.bin -serial mon:stdio -s -S";
