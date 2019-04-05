//
// Created by auriga on 03/04/19.
//

#include "manctl.h"
#include "drive/commdrv.h"

#define SZ_PACKET 16

static TaskHandle manctlHandle = nullptr;

typedef struct {
    Base_To_Remote_Pkt_T lastPacket;
    bool packetSeen;
    int V;
    int r;
} manCtlPacket;

long abs(long x){
    return (x > 0) ? x : (-1*x);
}

void manctlTask(void * state){
    int recv = uart2_bytes_received();

    manCtlPacket * packet = (manCtlPacket *) state;
    packet->packetSeen = false;

    if(recv > SZ_PACKET) {
        Roomba_ConfigSpotLED(LED_ON);

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
            if (now == '/')
                scount += 1;
            recvstr[curr - start] = now;
        }

        uart2_reset_receive();

        if (scount == 4) {
            base_to_remote_string_to_struct(recvstr, &(packet->lastPacket));
            Roomba_ConfigDirtDetectLED(LED_OFF);

            int rotSpeed = packet->lastPacket.rotate;
            int fwdSpeed = packet->lastPacket.direction;
            if (rotSpeed == 0) {
                packet->V = fwdSpeed;
                packet->r = 0x8000;
            } else if (fwdSpeed == 0) {
                packet->V = rotSpeed;
                packet->r = -1;
            } else {
                packet->V = (fwdSpeed + rotSpeed) / 2;
                packet->r = 500 / rotSpeed;
            }
            packet->packetSeen = true;
        } else {
            Roomba_ConfigDirtDetectLED(LED_ON);
        }
    }
    else {
        Roomba_ConfigDirtDetectLED(LED_OFF);
        Roomba_ConfigSpotLED(LED_OFF);
    }
}

void manctlStart(Timing_t when){
    uart_start(UART_38400);
    uart2_start(UART_9600);

    if(!manctlHandle){
        manctlHandle = OS_CreateTask(manctlTask, nullptr, when);
    }
}

bool haveManualControl(){
    return ((manCtlPacket *)OS_GetTaskState(manctlHandle)->state)->packetSeen;
}

int readManualV(){
    return ((manCtlPacket *)OS_GetTaskState(manctlHandle)->state)->V;
}

int readManualR(){
    return ((manCtlPacket *)OS_GetTaskState(manctlHandle)->state)->r;
}