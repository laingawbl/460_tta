#include "rtos/tta.h"
#include "uart/uart.h"
#include "drive/lightdrv.h"

void uartSendLightReading(void * state){
    bool reading = readLight();
    if(reading)
        uart_sendstr("hit!");
    else
        uart_sendstr("miss!");
}

int main(){

    uart_start();
    uart_sendstr("UART started");

    lightDriverStart();
    OS_CreateTask(uartSendLightReading, nullptr, {50, 25});
    OS_Run();

    uart_sendstr("test ended");

    return 0;
}