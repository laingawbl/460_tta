/**
 * @file   uart.c
 * @author Justin Tanner
 * @date   Sat Nov 22 21:32:03 2008
 *
 * @brief  UART Driver targetted for the AT90USB1287
 *
 */

#ifndef __UART_H__
#define __UART_H__

#include "../lib/mcu.h"

typedef enum _uart_bps
{
    UART_9600,
    UART_19200,
    UART_38400,
    UART_57600,
    UART_DEFAULT,
} UART_BPS;

#define UART_BUFFER_SIZE    32

void uart_start(UART_BPS bitrate);
void uart1_start(UART_BPS bitrate);

void uart_sendchar(char data);
void uart1_sendchar(char data);
void uart_sendstr(char *data);

uint8_t uart_get_byte(int index);
uint8_t uart_bytes_received(void);
void uart_reset_receive(void);

uint8_t uart1_get_byte(int index);
uint8_t uart1_bytes_received(void);
void uart1_reset_receive(void);

void utos(int16_t from, char * to);

#endif