//
// Created by auriga on 11/03/19.
//

#include "../src/tta.h"

void uart_sendstr(char *str);

struct dummy {
    int value;
};

/*
 * Expected result: the UART0 prints "inc;chk;inc.." at 1-second intervals, and "once!" every 6 cycles
 */

void once(void *state){
    Enable_Interrupt();
    uart_sendstr((char*)"\tonce!\n");
    Disable_Interrupt();
}

void pong(void *state){

    ((struct dummy *)state)->value += 1;

    Enable_Interrupt();
    uart_sendstr((char*)"inc;");
    Disable_Interrupt();
}

void ping(void *state){

    struct dummy * theData = (struct dummy *)state;

    Enable_Interrupt();
    uart_sendstr((char*)"chk;");
    Disable_Interrupt();

    if((theData->value >= 6)){
        theData->value = 0;
        OS_CreateSingleTask(once, nullptr, 100);
    }
}

int main(void){
    TaskHandle pingHandle = OS_CreateTask(ping, nullptr, {400, 100});

    // instead of a new block, use ping's existing block
    OS_CreateTask(pong, OS_GetTaskState(pingHandle), {400, 300});
    OS_Run();
}