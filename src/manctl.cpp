//
// Created by auriga on 03/04/19.
//

#include "manctl.h"

static TaskHandle manctlHandle = nullptr;

void manctlTask(void * state){
    Roomba_Drive(50, 50);
}

void manctlStart(Timing_t when){
    Roomba_Init();

    if(!manctlHandle){
        manctlHandle = OS_CreateTask(manctlTask, nullptr, when);
    }
}