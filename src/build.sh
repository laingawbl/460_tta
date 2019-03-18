#!/bin/sh
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o tta.o tta.cpp
avr-g++  -mmcu=atmega2560 -o active.elf *.o
avr-objcopy -j .text -j .data -O ihex active.elf active.flash.hex
avrdude -v -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:active.flash.hex
stty -F /dev/ttyACM0 38400 && cat /dev/ttyACM0

