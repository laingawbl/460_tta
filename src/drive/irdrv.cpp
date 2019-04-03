//
// Created by auriga on 02/04/19.
//

#include "irdrv.h"
#include "../uart/uart.h"
#include "roomba_sci.h"
#include "roomba_uart.h"

static TaskHandle irDrvTaskHandle = nullptr;

typedef struct {
    bool hit = false;
} irReading_t;

void irDriverTask(void * state) {
    uart1_sendchar(SENSORS);
    uart1_sendchar(13); // virtual wall packet
}

void irDriveRStart(Timing_t when) {
    if(!irDrvTaskHandle)
        irDrvTaskHandle = OS_CreateTask(irDriverTask, nullptr, when);
}

bool readIR() {
    return ((irReading_t *)OS_GetTaskState(irDrvTaskHandle)->state)->hit;
}

ISR(USART1_RX_vect) {
    char input = UDR1;
    irReading_t * reading = ((irReading_t *)OS_GetTaskState(irDrvTaskHandle)->state);
    if(input == 0)
        reading->hit = false;
    else
        reading->hit = true;
}