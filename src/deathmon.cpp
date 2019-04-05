//
// Created by auriga on 05/04/19.
//

#include "deathmon.h"

#define DEATH_THRESH 10

typedef struct {
    int deathCounter = 0;
    bool DEAD = false;
    bool HALT = false;
} deathMonState;

static TaskHandle deathMonHandle = nullptr;
static TaskHandle modeSwitchHandle = nullptr;

void deathMonTask(void * state){
    char str[7];
    utos(readLightLevel(), str);
    uart_sendstr(str);
    uart_sendchar('\n');

    deathMonState * info = (deathMonState *) state;
    if(!info->DEAD){
        if(readLight() | readIR()){
            uart_sendstr("\tDEATHMON: ouch!\n");
            info->deathCounter++;
        }
        else {
            uart_sendstr("\tDEATHMON: I'm OK\n");
            info->deathCounter = 0;
        }
        if(info->deathCounter >= DEATH_THRESH) {
            uart_sendstr("\tDEATHMON: robt dead!\n");
            Roomba_Drive(0, 0);
            info->DEAD = true;
            Roomba_ConfigSpotLED(LED_ON);
        }
    }
}

void modeSwitchTask(void * state){
    deathMonState * info = (deathMonState *) state;
    info->HALT = !(info->HALT);
    Roomba_ConfigSpotLED((info->HALT) ? LED_ON : LED_OFF);
}

void Roomba_DriveWithCutout(uint16_t vel, uint16_t rad){
    deathMonState * info = ((deathMonState *) OS_GetTaskState(deathMonHandle)->state);
    if(info->DEAD) {
        Roomba_Drive(0, 0);
    }
    else {
        if(info->HALT){
            if(rad){
                Roomba_Drive(vel, -1);
            }
            else {
                Roomba_Drive(0, 0);
            }
        }
        else{
            Roomba_Drive(vel, rad);
        }
    }

}

void deathMonStart(Timing_t when){
    Roomba_ConfigSpotLED(LED_OFF);

    if(!deathMonHandle){
        if(modeSwitchHandle)
            deathMonHandle = OS_CreateTask(deathMonTask, OS_GetTaskState(modeSwitchHandle), when);
        else
            deathMonHandle = OS_CreateTask(deathMonTask, nullptr, when);
    }
}

void modeSwitchStart(Timing_t when){

    if(!modeSwitchHandle){
        if(deathMonHandle)
            modeSwitchHandle = OS_CreateTask(modeSwitchTask, OS_GetTaskState(deathMonHandle), when);
        else
            modeSwitchHandle = OS_CreateTask(modeSwitchTask, nullptr, when);
    }
}