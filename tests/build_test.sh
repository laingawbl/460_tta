#!/bin/sh

pname="${1%.*}"
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o uart.o ../src/uart/uart.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o irdrv.o ../src/drive/irdrv.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o commdrv.o ../src/drive/commdrv.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o lightdrv.o ../src/drive/lightdrv.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o roomba.o ../src/lib/roomba.cpp

avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o deathmon.o ../src/deathmon.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o manctl.o ../src/manctl.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o autoctl.o ../src/autoctl.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o tta.o ../src/rtos/tta.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o turretdrv.o ../src/drive/turretdrv.cpp
avr-g++ -c -std=c++0x -Os -mmcu=atmega2560 -Wa,--gstabs -o "$pname".o "$pname".cpp
avr-g++  -mmcu=atmega2560 -o "$pname".elf *.o
avr-objcopy -j .text -j .data -O ihex "$pname".elf "$pname".flash.hex
avrdude -v -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:"$pname".flash.hex
rm *.o
rm *.hex
rm *.elf
stty -F /dev/ttyACM0 38400 && cat /dev/ttyACM0
