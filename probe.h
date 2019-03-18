//
// Created by auriga on 11/03/19.
//

#ifndef __PROBE_H__
#define __PROBE_H__

#include <avr/io.h>

#define PIN_PORT    PORTB
#define PIN_DDR     DDRB
#define PIN_10      1 << PORTB4
#define PIN_11      1 << PORTB5
#define PIN_12      1 << PORTB6
#define PIN_13      1 << PORTB7

#define PROBE_1_IN_KERNEL       (PIN_10)
#define PROBE_2_IDLING          (PIN_11)
#define PROBE_3_IN_NC_TASK      (PIN_10) | (PIN_11)
#define PROBE_4_IN_TIMED_TASK   (PIN_12)
#define PROBE_5_DISPATCH        (PIN_12) | (PIN_10)
#define PROBE_15_ABORT          (PIN_13) | (PIN_12) | (PIN_11) | (PIN_10)


void pinStart(unsigned int pin) {
    PIN_DDR |= pin;
}

void pinSet(unsigned int pins){
    PIN_PORT = pins;
}

void pinWrite(unsigned int pin, bool sig) {
    if(sig)
        PIN_PORT |= pin;
    else
        PIN_PORT &= ~(pin);
}

#endif
