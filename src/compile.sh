#!/bin/sh

pname="${1%.*}"
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o "$pname".o "$pname".cpp
avr-g++  -mmcu=atmega2560 -o "$pname".elf *.o
avr-objcopy -j .text -j .data -O ihex "$pname".elf "$pname".flash.hex
