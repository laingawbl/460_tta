//
// Created by auriga on 10/03/19.
//

#ifndef __ERR_H__
#define __ERR_H__

#include <avr/pgmspace.h>

typedef enum {
    /* Timing errors */

    // More than MAXJITTER_COUNT tasks experienced jitter
    E_TOO_MANY_JITTERED_TASKS,

    // A task was late to run by more than MAXJITTER_TICKS, crossing the line from jitter to serious delay
    E_TASK_DEADLINE_MISSED,

    // The TIMER5 counter, normally zeroed out by an interrupt every few seconds, overflowed
    E_TICK_UPDATE_WATCHDOG,

    /* Task errors */

    // Too many tasks (more than MAXTASKS) were created
    E_TOO_MANY_TASKS,

    // Too many states (more than MAXTASKS) were created
    // This should never happen, as tasks should either use an existing state block or obtain exactly one new block
    E_TOO_MANY_STATES
} ECODE;

#define ECODELength 22

const char *ECODEName0 = "Too many jitters    \n";
const char *ECODEName1 = "Task deadline missed\n";
const char *ECODEName2 = "Timer watchdog fired\n";
const char *ECODEName3 = "Too many task blocks\n";
const char *ECODEName4 = "Too many stateblocks\n";
const char *const ECODENames[] = {ECODEName0, ECODEName1, ECODEName2, ECODEName3, ECODEName4};

#endif
