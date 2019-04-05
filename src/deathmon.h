/**
 * @file deathmon.h
 * @brief Roomba death-condition monitor
 *
 * DEATHMON watches the output from LIGHTDRV and IRDRV to monitor whether the Roomba has been hit by a laser or crossed
 * the IR river (respectively); if either of these conditions are satisfied, DEATHMON will put the robot into a dead
 * state.
 *
 * TODO: maybe turn the Roomba status LED red if dead?
 */

#include "rtos/tta.h"
#include "lib/roomba.h"
#include "drive/irdrv.h"
#include "drive/lightdrv.h"

void Roomba_DriveWithCutout(uint16_t vel, uint16_t rad);

void deathMonStart(Timing_t when);

void modeSwitchStart(Timing_t when);