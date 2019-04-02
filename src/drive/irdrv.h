#ifndef __IRDRV_H__
#define __IRDRV_H__

/**
 * @file irdrv.h
 * @brief driver for reading the Roomba's IR sensors, by way of its general sensor output interface
 *
 * This driver describes a task (
 * TODO: pair of tasks?
 * ) which will query and store Roomba IR data, and commands for reading that data.
 */

#include<avr/io.h>

/**
 * TODO write the doxy!!
 */
void irDriveStart();

/**
 * Read each of the latest IR readings and determine if any of them saw the IR river signal.
 * @return true if the IR river beacons are detected by any sensor; false otherwise
 */
bool irRead();

#endif