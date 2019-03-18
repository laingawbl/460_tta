//
// Created by auriga on 11/03/19.
//

#include "../tta.h"

/*
 * Expected result: the watchdog timer fires and its interrupt aborts the OS after longRun exits
 */

void longRun(void *state){
    uint16_t i, j;
    for(j = 0; j < 250; j++) {
        for (i = 0; i < UINT16_MAX; i++) {
            asm volatile ("nop");
        }
    }
}

int main(void){
    OS_CreateSingleTask(longRun, nullptr, 100);
    OS_Run();
}