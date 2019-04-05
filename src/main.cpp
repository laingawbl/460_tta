#include "uart/uart.h"
#include "lib/roomba.h"
#include "lib/mcu.h"
#include "drive/turretdrv.h"
#include "drive/motiondrv.h"
#include "drive/commdrv.h"
#include "drive/irdrv.h"
#include "rtos/tta.h"
#include "manctl.h"

int main()
{
    uart_start(UART_57600);
    uart_sendstr("*** POWERING ON ***\n");
    uart_sendstr("*** STARTING IR DRIVER ***\n");
    irDriverStart((Timing_t) {50, 0});
    uart_sendstr("*** STARTING MANUAL CONTROL ***\n");
    manctlStart((Timing_t) {25, 0});
    //OS_Run();
}
