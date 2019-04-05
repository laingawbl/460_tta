//
// Created by auriga on 04/04/19.
//

#ifndef __AUTOCTL_H__
#define __AUTOCTL_H__

#include "rtos/tta.h"
#include "lib/roomba.h"
#include "uart/uart.h"
#include "drive/irdrv.h"
#include "drive/lightdrv.h"
#include "manctl.h"

void autoctlTask(void *);

void autoctlStart(Timing_t when);

#endif //PROJ2_AUTOCTL_H
