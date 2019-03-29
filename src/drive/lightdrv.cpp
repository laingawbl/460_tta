//
// Created by auriga on 25/03/19.
//

#include "lightdrv.h"
#include "../rtos/tta.h"

static TaskHandle lightDrvTaskHandle = nullptr;

struct lightDrvReading {
    bool hit;
};

void _lightDrvTask(void * state){
    ((struct lightDrvReading *) state)->hit = LIGHTDRV_INPUT & (1<<LIGHTDRV_PIN);
}

void lightDriverStart(){
    // set pin data direction to 0 (input)
    LIGHTDRV_DDR &= ~(1<<LIGHTDRV_PIN);

    // start only one instance of LIGHTDRV
    if(!lightDrvTaskHandle)
        OS_CreateTask(_lightDrvTask, nullptr, {50, 0});
}

bool readLight(){
    StateHandle handle = OS_GetTaskState(lightDrvTaskHandle);
    if(handle)
        // ... return the "hit" field, of the casted-to-struct state member, of the stateBlock for which we have a handle
        //TODO: actually read this, right now it'll basically always return true
        return ((struct lightDrvReading *) handle->state)->hit;

    //TODO: raise application-level error if we can't retrieve the LIGHTDRV state handle
    return true;
}