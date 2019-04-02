//
// Created by auriga on 02/04/19.
//

#ifndef __ROOMBA_UART_H__
#define __ROOMBA_UART_H__

#define DIN_PIN PC5
#define DIN_PORT PORTC
#define DIN_DDR DDRC

#include "../rtos/tta.h"

void

void din_wake_high(void * state){
    DIN_PORT |= (1 << DIN_PIN);
}

void din_wake_low(void * state){
    DIN_PORT &= ~(1 << DIN_PIN);
}

/**
 * @brief orchestrate various microtasks for starting the Roomba's Open Interface (DIN port) communications
 *
 * To start talking to the iRobot over its DIN port, a particular song and dance has to be done, which, among other
 * things, involves numerous waits of between 20 - 2000ms. Rather than implementing something like delay(), I have
 * structured this as a chain of small TTA tasks.
 *
 * Note that these tasks are called in reverse order that they appear in this file, starting with roomba_start() at the
 * bottom, and proceeding upward.
 */
void roomba_start(){
    DIN_DDR |= (1 << DIN_PIN);

    // We save the TaskHandle so that one isn't created for each task
    TaskHandle dummy = OS_CreateSingleTask(din_wake_low, nullptr, 0);

    // 2-second wake pulse
    OS_CreateSingleTask(din_wake_high, dummy, 500);
    OS_CreateSingleTask(din_wake_low,  dummy, 2500);

    // set DIN rate 19200 baud with 3 pulses
    // pulse 1
    OS_CreateSingleTask(din_wake_high, dummy, 2550);
    OS_CreateSingleTask(din_wake_low,  dummy, 2600);

    // pulse 2
    OS_CreateSingleTask(din_wake_high, dummy, 2650);
    OS_CreateSingleTask(din_wake_low,  dummy, 2700);

    // pulse 3
    OS_CreateSingleTask(din_wake_high, dummy, 2750);
    OS_CreateSingleTask(din_wake_low,  dummy, 2800);
}

#endif //PROJ2_ROOMBA_UART_H
