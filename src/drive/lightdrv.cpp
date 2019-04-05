//
// Created by auriga on 25/03/19.
//

#include "lightdrv.h"

static TaskHandle lightDrvTaskHandle = nullptr;

typedef struct {
    bool hit = false;
    int rawVal = 0;
} lightReading_t;

void _lightDrvTask(void * state){
    uint8_t high, low;
    int result;
    lightReading_t *reading = (lightReading_t *) state;

    // select channel (mod 8, as high bit is handled by MUX5)
    ADMUX |= (LIGHTDRV_PIN & 0x07);

    // start comparison by setting ADSC
    ADCSRA |= (1 << ADSC);

    // wait for ADSC to clear on conversion finish
    while(ADCSRA & (1 << ADSC));

    low = ADCL;
    high = ADCH;
    result = (high << 8) | low;

    reading->rawVal = result;
    reading->hit = (result > LIGHTDRV_COMPARE);
}

void lightDriverStart(Timing_t when){
    // set pin data direction to 0 (input)
    LIGHTDRV_DDR &= ~(1<<LIGHTDRV_PIN);

    // use Vcc as reference
    ADMUX = (1 << REFS0);

    // 128x prescale clock, we are in no rush, and enable ADC
    ADCSRA |= ( 1 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 ) | (1 << ADEN);

    // start only one instance of LIGHTDRV
    if(!lightDrvTaskHandle)
        lightDrvTaskHandle = OS_CreateTask(_lightDrvTask, nullptr, when);
}

bool readLight(){
    lightReading_t *reading = (lightReading_t *) OS_GetTaskState(lightDrvTaskHandle)->state;
    return reading->hit;
}

int readLightLevel(){
    lightReading_t *reading = (lightReading_t *) OS_GetTaskState(lightDrvTaskHandle)->state;
    return reading->rawVal;
}
