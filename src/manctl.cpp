//
// Created by auriga on 03/04/19.
//

#include "manctl.h"
#include "drive/commdrv.h"


#define SZ_PACKET 16


static TaskHandle manctlHandle = nullptr;

void manctlTask(void * state){

    Base_To_Remote_Pkt_T recvPacket;

    while (uart1_bytes_received() < SZ_PACKET);
    recvPacket.laser_power  = (uart1_get_byte(1) << 8) | (uart1_get_byte(2));
    recvPacket.turret_x_pos = (uart1_get_byte(4) << 8) | (uart1_get_byte(5));
    recvPacket.turret_y_pos = (uart1_get_byte(7) << 8) | (uart1_get_byte(8));
    recvPacket.rotate       = (uart1_get_byte(10) << 8) | (uart1_get_byte(11));
    recvPacket.direction    = (uart1_get_byte(13) << 8) | (uart1_get_byte(14));

    char * pretty = pretty_print_base_to_remote_struct(&recvPacket);

    uart_sendstr(pretty);

    free(pretty);

    uart1_reset_receive();
}

void manctlStart(Timing_t when){
    Roomba_Init();
    uart1_start(UART_9600);
    uart_start(UART_38400);

    if(!manctlHandle){
        manctlHandle = OS_CreateTask(manctlTask, nullptr, when);
    }
}