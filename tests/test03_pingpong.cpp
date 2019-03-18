//
// Created by auriga on 11/03/19.
//

#include "../tta.h"

void uart_sendstr(char *str);

/*
 * Expected result: the UART0 prints "1;2;3;1;2;3..." on a 200ms cycle
 */

struct dummy {
    int value;
};

void ping(void *state){
    Enable_Interrupt();
    uart_sendstr((char*)"1;");
    Disable_Interrupt();
}

void pong(void *state){
    Enable_Interrupt();
    uart_sendstr((char*)"2;");
    Disable_Interrupt();
}

void pang(void *state){
    Enable_Interrupt();
    uart_sendstr((char*)"3;");
    Disable_Interrupt();
}

int main(void){
    OS_CreateTask(ping, nullptr, {200, 260});
    OS_CreateTask(pong, nullptr, {200, 320});
    OS_CreateTask(pang, nullptr, {200, 380});
    OS_Run();
}