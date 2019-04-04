/**
 * @file: manctl.h
 * @brief: manual Roomba control daemon
 *
 * MANCTL awaits packets from the base-station via Bluetooth (from COMMDRV)
 * and demultiplexes them into control variables. If a packet is received,
 * MANCTL will override the semi-autonomous control daemons from Phase 2
 * (TURRETDRV and MOTIONDRV).
 */

#include "rtos/tta.h"
#include "lib/roomba.h"
#include "uart/uart.h"

void manctlTask(void *);

void manctlStart(Timing_t when);