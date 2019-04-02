//
// Created by auriga on 02/04/19.
//

#include "irdrv.h"
#include "../rtos/tta.h"
#include "roomba_sci.h"

static TaskHandle irDrvTaskHandle = nullptr;

typedef struct {
    bool hit = false;
} irReading_t;

void irDriveTask(void * state) {

}

void irDriveStart() {

}

bool irRead() {

}