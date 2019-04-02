//
// Created by auriga on 25/03/19.
//

#include "lightdrv.h"
#include "../uart/uart.h"
#include "../rtos/tta.h"

static TaskHandle lightDrvTaskHandle = nullptr;

struct lightDrvReading {
    bool hit = false;
    int rawVal = 0;
};

void _lightDrvTask(void * state){
    uint8_t high, low;
    int result;

    // select channel (mod 8, as high bit is handled by MUX5)
    ADMUX |= (LIGHTDRV_PIN & 0x07);

    // start comparison by setting ADSC
    ADCSRA |= (1 << ADSC);

    // wait for ADSC to clear on conversion finish
    while(ADCSRA & (1 << ADSC));

    low = ADCL;
    high = ADCH;
    result = (high << 8) | low;

    char rawLevelStr[7];
    utos(result, rawLevelStr);
    uart_sendstr(rawLevelStr);
    uart_sendstr("\t");

    ((struct lightDrvReading *) state)->rawVal = 42;
    ((struct lightDrvReading *) state)->hit = (result > LIGHTDRV_COMPARE);
}

void lightDriverStart(){
    // set pin data direction to 0 (input)
    LIGHTDRV_DDR &= ~(1<<LIGHTDRV_PIN);

    // use Vcc as reference
    ADMUX = (1 << REFS0);

    // 128x prescale clock, we are in no rush, and enable ADC
    ADCSRA |= ( 1 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 ) | (1 << ADEN);

    // start only one instance of LIGHTDRV
    if(!lightDrvTaskHandle)
        OS_CreateTask(_lightDrvTask, nullptr, {50, 0});
}

bool readLight(){
    StateHandle handle = OS_GetTaskState(lightDrvTaskHandle);
    if(handle)
        return ((struct lightDrvReading *) handle->state)->hit;

    //TODO: raise application-level error if we can't retrieve the LIGHTDRV state handle
    return false;
}

int readLightLevel(){
    StateHandle handle = OS_GetTaskState(lightDrvTaskHandle);
    if(handle)
        return ((struct lightDrvReading *) handle->state)->rawVal;

    //TODO: raise application-level error if we can't retrieve the LIGHTDRV state handle
    return -1;

}
