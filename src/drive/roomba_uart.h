//
// Created by auriga on 02/04/19.
//

#ifndef __ROOMBA_UART_H__
#define __ROOMBA_UART_H__

#define DIN_PIN PC5
#define DIN_PORT PORTC
#define DIN_DDR DDRC

#include "../rtos/tta.h"
#include "roomba_sci.h"

// YES, this is a dumb way to do things, but *handwaves* TTA

typedef struct{
    int command;
} dinCommand_t;

void din_cmdtask(void * state){
    uart1_sendchar(((dinCommand_t *)state)->command);
}

void din_wake_high(void * state){
    DIN_PORT |= (1 << DIN_PIN);
}

void din_wake_low(void * state){
    DIN_PORT &= ~(1 << DIN_PIN);
}

void roomba_sendCmdAt(int what, int when){
    TaskHandle cmdTask = OS_CreateSingleTask(din_cmdtask, nullptr, when);
    StateHandle cmdState = OS_GetTaskState(cmdTask);
    ((dinCommand_t *)cmdState->state)->command = what;
}

/**
 * @brief orchestrate various microtasks for starting the Roomba's Open Interface (DIN port) communications
 *
 * To start talking to the iRobot over its DIN port, a particular song and dance has to be done, which, among other
 * things, involves numerous waits of between 20 - 2000ms. Rather than implementing something like delay(), I have
 * structured this as a chain of small TTA tasks.
 */
void roomba_start(){
    DIN_DDR |= (1 << DIN_PIN);
    uart1_start();

    // We save the TaskHandle so that one isn't created for each task
    TaskHandle firstTask = OS_CreateSingleTask(din_wake_low, nullptr, 0);
    StateHandle dummy = OS_GetTaskState(firstTask);

    // 2-second wake pulse
    OS_CreateSingleTask(din_wake_high, dummy, 500);

    // set DIN rate 19200 baud with 3 pulses
    // pulse 1
    OS_CreateSingleTask(din_wake_low,  dummy, 2500);
    OS_CreateSingleTask(din_wake_high, dummy, 2550);

    // pulse 2
    OS_CreateSingleTask(din_wake_low,  dummy, 2600);
    OS_CreateSingleTask(din_wake_high, dummy, 2650);

    // pulse 3
    OS_CreateSingleTask(din_wake_low,  dummy, 2700);
    OS_CreateSingleTask(din_wake_high, dummy, 2750);

    // send the START command to the Roomba
    roomba_sendCmdAt(START, 2800);

    // put the roomba into safe mode with CONTROL
    roomba_sendCmdAt(CONTROL, 2820);
}

#endif //PROJ2_ROOMBA_UART_H
