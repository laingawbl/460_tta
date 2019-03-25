//
// Created by auriga on 11/03/19.
//

#include "../src/rtos/tta.h"

void uart_sendstr(char *str);

/*
 * Expected result: the UART0 prints "even; inc; odd; inc; even; inc; odd"...
 */

struct dummy {
    int value;
};

void pong(void *state){

    ((struct dummy *)state)->value += 1;

    Enable_Interrupt();
    uart_sendstr((char*)"inc\t");
    Disable_Interrupt();
}

void ping(void *state){

    struct dummy * theData = (struct dummy *)state;

    if((theData->value % 2) == 0){
        Enable_Interrupt();
        uart_sendstr((char*)"evn\n");
        Disable_Interrupt();
    }
    else {
        Enable_Interrupt();
        uart_sendstr((char*)"odd\n");
        Disable_Interrupt();
    }


}

int main(void) {
    TaskHandle pingHandle = OS_CreateTask(ping, nullptr, {200, 0});

    // instead of a new block, use ping's existing block
    OS_CreateTask(pong, OS_GetTaskState(pingHandle), {200, 100});

    OS_Run();
}
