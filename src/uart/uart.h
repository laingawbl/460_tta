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
#define BAUD 38400
#define BUFF_LEN 63
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)

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

#endif

