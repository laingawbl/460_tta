#!/bin/sh
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o uart.o ../src/uart/uart.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o basestation.o basestation.cpp
avr-g++  -mmcu=atmega2560 -o base.elf *.o
avr-objcopy -j .text -j .data -O ihex base.elf base.flash.hex
avrdude -v -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:base.flash.hex
stty -F /dev/ttyACM0 38400 && cat /dev/ttyACM0

