//
// Created by auriga on 02/04/19.
//

#include "irdrv.h"

static TaskHandle irDrvTaskHandle = nullptr;

typedef struct {
    bool hit = false;
    bool wall = false;
} irReading_t;

void irDriverTask(void * state) {
    // wait for uart1 by polling, sure who cares
    uint8_t recv = uart1_bytes_received();
    if (recv > 2) {
        bool hitReading = uart1_get_byte(0);
        bool wallReading = uart1_get_byte(1);
        ((irReading_t *) OS_GetTaskState(irDrvTaskHandle)->state)->hit = hitReading;
        ((irReading_t *) OS_GetTaskState(irDrvTaskHandle)->state)->wall = wallReading;

        // clean up UART1's receive buffer; god knows who for, nobody else is using it
        uart1_reset_receive();
    }

    uart1_sendchar(149);
    uart1_sendchar(2);  // number of bytes in query
    uart1_sendchar(7);   // bumper/wheeldrop query
    uart1_sendchar(13); // virtual wall packet
    //TODO: un-magicnumber the previous line
}

void irDriverStart(Timing_t when) {
    uart_sendstr("start!\n");
    if(!irDrvTaskHandle)
        irDrvTaskHandle = OS_CreateTask(irDriverTask, nullptr, when);
}

bool readIR() {
    return ((irReading_t *)OS_GetTaskState(irDrvTaskHandle)->state)->wall;
}

bool readBumper() {
    return ((irReading_t *)OS_GetTaskState(irDrvTaskHandle)->state)->hit;
}