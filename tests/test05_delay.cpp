//
// Created by auriga on 11/03/19.
//

#include "../tta.h"

/*
 * Expected result: the OS aborts with a "deadline missed" error
 */

void uart_sendstr(char *str);

void longRun(void *state){
    uint16_t i, j;
    for (j = 0; j < 100; j++) {
        for (i = 0; i < UINT16_MAX; i++) {
            asm volatile ("nop");
        }
    }
    Enable_Interrupt();
    uart_sendstr((char*)"longRun done\n");
    Disable_Interrupt();
}

void ping(void *state){
    Enable_Interrupt();
    uart_sendstr((char*)"ping!\n");
    Disable_Interrupt();
}

int main(void){
    OS_CreateTask(longRun, nullptr, {10, 0});
    OS_CreateTask(ping, nullptr, {10, 1});
    OS_Run();
}