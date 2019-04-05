#include "uart/uart.h"

int main()
{
    uart_start(UART_57600);
    uart_sendstr("Hello!\n");
}
