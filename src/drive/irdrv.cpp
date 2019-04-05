//
// Created by auriga on 02/04/19.
//

#include "irdrv.h"

static TaskHandle irDrvTaskHandle = nullptr;

typedef struct {
    bool hit = false;
} irReading_t;

void irDriverTask(void * state) {
    uart1_sendchar(SENSORS);
    uart1_sendchar(13); // virtual wall packet
    //TODO: un-magicnumber the previous line

    // wait for uart1 by polling, sure who cares
    while(uart1_bytes_received() != 1);
    bool hitReading = uart1_get_byte(0);
    ((irReading_t *)OS_GetTaskState(irDrvTaskHandle)->state)->hit = hitReading;

    // clean up UART1's receive buffer; god knows who for, nobody else is using it
    uart1_reset_receive();
}

void irDriverStart(Timing_t when) {
    if(!irDrvTaskHandle)
        irDrvTaskHandle = OS_CreateTask(irDriverTask, nullptr, when);
}

bool readIR() {
    return ((irReading_t *)OS_GetTaskState(irDrvTaskHandle)->state)->hit;
}
