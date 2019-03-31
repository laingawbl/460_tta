#!/bin/sh
pname="${1%.*}";
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o "$pname".o "$pname".cpp
avr-g++  -mmcu=atmega2560 -o active.elf *.o
avr-objcopy -j .text -j .data -O ihex active.elf active.flash.hex
avrdude -v -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:active.flash.hex
stty -F /dev/ttyACM0 0:4:cbf:0:3:1c:7f:15:4:0:0:0:11:13:1a:0:12:f:17:16:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0 && cat /dev/ttyACM0

