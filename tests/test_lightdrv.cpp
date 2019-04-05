#include <avr/io.h>

#include "../src/uart/uart.h"
#include "../src/rtos/tta.h"
#include "../src/drive/lightdrv.h"

void raiseLEDOnIRReading(void * state){

    bool reading = readLight();
    char rawLevelStr[7];

    if(reading) {
        PORTB |= (1 << PORTB7);
        PORTB &= ~(1 << PORTB6);
    }
    else {
        PORTB &= ~(1 << PORTB7);
        PORTB |= (1 << PORTB6);
    }

    utos(readLightLevel(), rawLevelStr);
    uart_sendstr(rawLevelStr);
    uart_sendstr("\n");
}

int main(){

    DDRB |= (1 << PORTB7);
    DDRB |= (1 << PORTB6);

    uart_start(UART_38400);

    lightDriverStart();
    OS_CreateTask(raiseLEDOnIRReading, nullptr, {50, 25});

    OS_Run();

    return 0;
}
