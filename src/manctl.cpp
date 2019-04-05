//
// Created by auriga on 03/04/19.
//

#include "manctl.h"
#include "drive/commdrv.h"


#define SZ_PACKET 16


static TaskHandle manctlHandle = nullptr;

long abs(long x){
    return (x > 0) ? x : (-1*x);
}

void drive_by_speed(int fwdSpeed, int rotSpeed){
    int V, r;
    if(rotSpeed == 0){
        V = fwdSpeed;
        r = 0x8000;
    }
    else if(fwdSpeed == 0) {
        V = rotSpeed;
        r = 1;
    }
    else {
        V = (fwdSpeed + rotSpeed) / 2;
        r = 500 / rotSpeed;
    }

    char str[7];
    utos(V, str);
    uart_sendstr(str);
    uart_sendchar('\n');
    utos(r, str);
    uart_sendstr(str);
    uart_sendchar('\n');

    Roomba_Drive(V, r);
}

void manctlTask(void * state){
    int recv = uart2_bytes_received();

    if(recv > SZ_PACKET) {
        Roomba_ConfigSpotLED(LED_ON);

        Base_To_Remote_Pkt_T *recvPacket = (Base_To_Remote_Pkt_T *) state;

        char recvstr[32];
        int start = 0, end;
        int curr = 0;

        // yes we are counting slash characters lolwatever
        int scount = 0;

        while (uart2_get_byte(start) != '^' && start < recv)
            start++;

        end = start;
        while (uart2_get_byte(end) != '$' && start < recv) {
            end++;
        }

        for (curr = start; curr <= end; curr++) {
            char now = uart2_get_byte(curr);
            //uart_sendchar(now);
            if(now == '/')
                scount += 1;
            recvstr[curr - start] = now;
        }

        uart2_reset_receive();

        //uart_sendchar('\n');

        if(scount == 4){
            base_to_remote_string_to_struct(recvstr, recvPacket);
            Roomba_ConfigDirtDetectLED(LED_OFF);

            drive_by_speed(recvPacket->direction, recvPacket->rotate);
        }
        else {
            /*
            char str[7];
            utos(scount, str);
            uart_sendstr(str);
            uart_sendchar('\n');*/
            Roomba_ConfigDirtDetectLED(LED_ON);
        }
    }
    else {
        Roomba_ConfigDirtDetectLED(LED_OFF);
        Roomba_ConfigSpotLED(LED_OFF);
    }
}

void manctlStart(Timing_t when){
    Roomba_Init();
    uart_start(UART_38400);
    uart2_start(UART_9600);

    if(!manctlHandle){
        manctlHandle = OS_CreateTask(manctlTask, nullptr, when);
    }
}