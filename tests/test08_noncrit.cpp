//
// Created by auriga on 13/03/19.
//

//
// Created by auriga on 11/03/19.
//

#include "../tta.h"

void uart_sendstr(char *str);

/*
 * Expected result: the UART0 prints "1;2;3;1;2;3..." on a 200ms cycle,
 * and "slowpoke" whenever it gets a chance. if "ping" and "pong" are scheduled
 * close enough together, the noncritical tasks shouldn't run until "pong" is done.
 */

void slow(void *state){
    Enable_Interrupt();
    uart_sendstr((char*)"slowpoke!\n");
    Disable_Interrupt();
}

void slow2(void *state){
    Enable_Interrupt();
    uart_sendstr((char*)"pokeslow!\n");
    Disable_Interrupt();
}

void ping(void *state){
    Enable_Interrupt();
    uart_sendstr((char*)"1;");
    Disable_Interrupt();

    OS_CreateNonCriticalTask(slow, nullptr);
    OS_CreateNonCriticalTask(slow2, nullptr);
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
    OS_CreateTask(ping, nullptr, {200, 220});
    OS_CreateTask(pong, nullptr, {200, 220});
    OS_CreateTask(pang, nullptr, {200, 300});
    OS_Run();
}