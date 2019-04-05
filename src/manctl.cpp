//
// Created by auriga on 03/04/19.
//

#include "manctl.h"
#include "drive/commdrv.h"
#include "drive/turretdrv.h"

#include <stdio.h>

#define SZ_PACKET 16

inline uint32_t _osGetTick(void);

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
            packet->packetSeen = true;

            base_to_remote_string_to_struct(recvstr, &(packet->lastPacket));
            Roomba_ConfigDirtDetectLED(LED_OFF);

            int rotSpeed = packet->lastPacket.rotate;
            int fwdSpeed = packet->lastPacket.direction;
            
            char str[128];
            sprintf(str, "OCR1A %i\nOCR1B %i\nX %i\nY %i\nRot %i\nfwd %i\n\n", OCR1A, OCR1B, packet->lastPacket.turret_x_pos, packet->lastPacket.turret_y_pos, rotSpeed, fwdSpeed);
            uart_sendstr(str);
            
            OCR1A += (packet->lastPacket.turret_x_pos) / 7;
            OCR1B += (packet->lastPacket.turret_y_pos) / 7;
            
            if(OCR1A > 2000)
                OCR1A = 2000;
            if(OCR1A < 1000)
                OCR1A = 1000;
            if(OCR1B > 2000)
                OCR1B = 2000;
            if(OCR1B < 1000)
                OCR1B = 1000;
                
            static int32_t laser_power = 10000;
            static int32_t laser_lastchange = 0;
            
            if((PORTB && _BV(PB4) && !packet->lastPacket.laser_power) || (!((PORTB && _BV(PB4)) && packet->lastPacket.laser_power)))
            {
                laser_lastchange = _osGetTick();
            }
            
            if(packet->lastPacket.laser_power)
            {
                PORTB |= _BV(PB4);
                laser_power -= _osGetTick() - laser_lastchange;
                laser_lastchange = _osGetTick();
                uart_sendstr("ON!\n");
            }
            else if(!packet->lastPacket.laser_power)
            {
                PORTB &= ~_BV(PB4);
            }
            
            if(laser_power < 0)
            {
                laser_power = 0;
                PORTB &= ~_BV(PB4);
            }
            
            sprintf(str, "Laser power: %i\n", laser_power);
            uart_sendstr(str);
            
            if (rotSpeed == 0) {
                if(fwdSpeed == 0){
                    packet->packetSeen = false;
                }
                packet->V = fwdSpeed;
                packet->r = 0x8000;
            } else if (fwdSpeed == 0) {
                packet->V = rotSpeed;
                packet->r = -1;
            } else {
                packet->V = (fwdSpeed + rotSpeed) / 2;
                packet->r = 500 / rotSpeed;
            }
        } else {
            Roomba_ConfigDirtDetectLED(LED_ON);
        }
    }
    else {
        Roomba_ConfigDirtDetectLED(LED_OFF);
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
