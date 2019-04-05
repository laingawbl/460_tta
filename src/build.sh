#!/bin/sh

avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o uart.o ./uart/uart.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o commdrv.o ./drive/commdrv.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o roomba.o ./lib/roomba.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o manctl.o ./manctl.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o tta.o ./rtos/tta.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o turretdrv.o ./drive/turretdrv.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o main.o main.cpp
avr-g++  -mmcu=atmega2560 -o main.elf *.o
avr-objcopy -j .text -j .data -O ihex main.elf main.flash.hex
avrdude -v -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:main.flash.hex
rm *.o
rm *.hex
rm *.elf
stty -F /dev/ttyACM0 57600 && cat /dev/ttyACM0

