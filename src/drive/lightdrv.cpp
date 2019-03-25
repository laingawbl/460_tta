//
// Created by auriga on 25/03/19.
//

#include "lightdrv.h"
#include "../rtos/tta.h"

static TaskHandle lightDrvTaskHandle = nullptr;

struct {
    bool hit
} lightDrvReading;

void _lightDrvTask(void * state){
    // (lightDrvReading)(state).hit = ...
}

void lightDriverStart(){
    // start only one instance of LIGHTDRV
    if(!lightDrvTaskHandleTask)
        OS_CreateTask(_lightDrvTask, nullptr, {period = 50, offset = 0});
}

bool readLight(){
    StateHandle handle = OS_GetTaskState(lightDrvTaskHandle);
    if(handle)
        return (lightDrvReading)(->state).hit;

    //TODO: raise application-level error if we can't retrieve the LIGHTDRV state handle
    return true;
}