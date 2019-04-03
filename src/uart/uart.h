//-------------------------------------------
/*
UART.c
2013 - Josh Ashby
joshuaashby@joshashby.com
http://joshashby.com
http://github.com/JoshAshby
freenode/#linuxandsci - JoshAshby
*/
//-------------------------------------------
#define F_CPU 16000000l

//-------------------------------------------
/*
UART.h
2013 - Josh Ashby
joshuaashby@joshashby.com
http://joshashby.com
http://github.com/JoshAshby
freenode/#linuxandsci - JoshAshby
*/
//-------------------------------------------
#ifndef __UART_H__
#define __UART_H__

//-------------------------------------------
//Macros (defines)
//-------------------------------------------
#define BAUD0 38400
#define BAUD1 19200

#define BAUD0_PRESCALE (((F_CPU / (BAUD0 * 16UL))) - 1)
#define BAUD1_PRESCALE (((F_CPU / (BAUD1 * 16UL))) - 1)

#define BUFF_LEN 63

//-------------------------------------------
//Prototypes
//-------------------------------------------
void uart_start(void);
void uart_sendint(uint8_t data);
void uart_sendint16(uint16_t data);
void uart_sendstr(char *data);

void utos(int16_t from, char * to);

//-------------------------------------------
//Variables
//-------------------------------------------
