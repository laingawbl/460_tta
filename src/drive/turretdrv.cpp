/*
 * Comms driver for bluetooth interface
 * Parses values and converts them to parameters.
 * Works on a host / slave model
*/

#define F_CPU 16000000l

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include<avr/io.h>
#include <util/delay.h>

void init_turret_pwm()
{
  //Set testFactor to 1 for real life or to a higher value
  //to speed up the simulation
  int testFactor=1; //ICR1 and OCR1A are divided by this value


  //Initialize PORTB

  PORTB=0b00100000;  //start with OC1A high
  DDRB= 0xFF;  //set OC1A to output (among others)

  ICR1=20000/testFactor;
  OCR1A=1000/testFactor; //set 1ms pulse  1000=1ms  2000=2ms
  OCR1B=1000/testFactor; //set 1ms pulse  1000=1ms  2000=2ms
  TCCR1A=(1<<COM1A1) | (1<<COM1B1);//COM1A1 Clear OCnA when match counting up,Set on 

  TCCR1B=(1<<WGM13)|(1<<CS11);// Phase and Freq correct ICR1=Top
}

void set_turret_x_val(unsigned int val) // Between 0 and 100
{
    OCR1A = 1000 + 1000 * ((float)val/100.0);
}

void set_turret_y_val(unsigned int val) // Between 0 and 100
{
    OCR1B = 1000 + 1000 * ((float)val/100.0);
}
