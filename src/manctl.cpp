//
// Created by auriga on 03/04/19.
//

#include "manctl.h"
#include "drive/commdrv.h"


#define SZ_PACKET 16


static TaskHandle manctlHandle = nullptr;

void manctlTask(void * state){
    int recv = uart_bytes_received();

    if(recv > SZ_PACKET) {

        Base_To_Remote_Pkt_T *recvPacket = (Base_To_Remote_Pkt_T *) state;

        char recvstr[32];
        int start = 0, end;
        int curr = 0;

        // yes we are counting slash characters lolwatever
        int scount = 0;

        while (uart_get_byte(start) != '^' && start < recv)
            start++;

        end = start;
        while (uart_get_byte(end) != '$' && start < recv) {
            end++;
        }

        for (curr = start; curr <= end; curr++) {
            char now = uart_get_byte(curr);
            if(now == '/')
                scount += 1;
            recvstr[curr - start] = now;
        }

        uart_reset_receive();

        if(scount == 4){
            base_to_remote_string_to_struct(recvstr, recvPacket);
            Roomba_ConfigStatusLED(GREEN);
            Roomba_Drive(recvPacket->direction, recvPacket->rotate);
        }
    }
    else{
        Roomba_ConfigStatusLED(RED);
    }
}

void manctlStart(Timing_t when){
    Roomba_Init();
    uart_start(UART_9600);

    if(!manctlHandle){
        manctlHandle = OS_CreateTask(manctlTask, nullptr, when);
    }
}