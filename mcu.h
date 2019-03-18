#ifndef __MCU_H__
#define __MCU_H__
/*!
 \file mcu.h
 \brief MCU-specific defines for the ATMega2560
*/

#define FCPU 16000000UL

#define ONE_SECOND_1024 15625

#define Disable_Interrupt() asm volatile ("cli"::)
#define Enable_Interrupt()  asm volatile ("sei"::)

#endif