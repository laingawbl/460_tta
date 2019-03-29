//
// Created by auriga on 25/03/19.
//

#include "lightdrv.h"
#include "../rtos/tta.h"

static TaskHandle lightDrvTaskHandle = nullptr;

struct lightDrvReading {
    bool hit;
};

void _lightDrvTask(void * state){
    uint8_t high, low;
    uint16_t result;

    // raise MUX5 if reading from channel A8 or above, clear it if reading from channels A0-A7
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((LIGHTDRV_PIN >> 3) & 0x01) << MUX5);

    // select channel (mod 8, as high bit is handled by MUX5)
    ADMUX |= (LIGHTDRV_PIN & 0x07);

    // start comparison by setting ADSC
    ADCSRA |= (1 << ADSC);

    // TODO: is a poll here OK? I have it on word-of-forum that ADC takes maximum 15 cycles to complete; watch for trouble
    // wait for ADSC to clear on conversion finish
    while(ADCSRA & (1 << ADSC));

    low = ADCL;
    high = ADCH;
    result = (high << 8) | low;

    ((struct lightDrvReading *) state)->hit = (result > LIGHTDRV_COMPARE);
}

void lightDriverStart(){
    // set pin data direction to 0 (input)
    LIGHTDRV_DDR &= ~(1<<LIGHTDRV_PIN);

    // enable ADC comparisons
    ADCSRA &= (1 << ADEN);

    // start only one instance of LIGHTDRV
    if(!lightDrvTaskHandle)
        OS_CreateTask(_lightDrvTask, nullptr, {50, 0});
}

bool readLight(){
    StateHandle handle = OS_GetTaskState(lightDrvTaskHandle);
    if(handle)
        return ((struct lightDrvReading *) handle->state)->hit;

    //TODO: raise application-level error if we can't retrieve the LIGHTDRV state handle
    return true;
}