#!/bin/sh
rm *.o

pname="${1%.*}"
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o tta.o ../src/rtos/tta.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o "$pname".o "$pname".cpp
avr-g++  -mmcu=atmega2560 -o "$pname".elf *.o
avr-objcopy -j .text -j .data -O ihex "$pname".elf "$pname".flash.hex
avrdude -v -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:"$pname".flash.hex
stty -F /dev/ttyACM0 38400 && cat /dev/ttyACM0

