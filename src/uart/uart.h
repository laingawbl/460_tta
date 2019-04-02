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

//-------------------------------------------
//Variables
//-------------------------------------------
char input_buffer[BUFF_LEN];

uint16_t read_spot;


#endif

//Got through and set up the registers for UART0
void uart_start(void) {
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0); //transmit side of hardware
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); //receive side of hardware

    UBRR0L = BAUD0_PRESCALE; //set the baud to BAUD0, have to split it into the two registers
    UBRR0H = (BAUD0_PRESCALE >> 8); //high end of baud register

    UCSR0B |= (1 << RXCIE0); //recieve data interrupt, makes sure we don't loose data

#if DEBUG
    uart_sendstr("0x04 - UART is up...");
#endif
}

void uart1_start(void) {
    UCSR1B |= (1 << RXEN1) | (1 << TXEN1); //transmit side of hardware
    UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11); //receive side of hardware

    UBRR1L = BAUD1_PRESCALE; //set the baud to BAUD1, have to split it into the two registers
    UBRR1H = (BAUD1_PRESCALE >> 8); //high end of baud register

    UCSR1B |= (1 << RXCIE1); //recieve data interrupt, makes sure we don't loose data
}

void utos(int16_t from, char * to){
    // char 48 = ascii '0'
    if(from < 0) {
        from *= -1;
        to[0] = '-';
    }
    else {
        to[0] = '+';
    }
    to[1] = ((from / 10000) % 10) + 48;
    to[2] = ((from / 1000)  % 10) + 48;
    to[3] = ((from / 100)   % 10) + 48;
    to[4] = ((from / 10)    % 10) + 48;
    to[5] = (from           % 10) + 48;
    to[6] = '\0';
}

void uart_sendint(uint8_t data) {
    /*
    Use this to send a 8bit long piece of data
    */
    while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
    UDR0 = data; //send the data
    while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
    //UDR0 = '\n';//send a new line just to be sure
}

void uart_sendint16(uint16_t data) {
    /*
    Use this to send a 16bit long piece of data
    */
    while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
    UDR0 = data;//send the lower bits
    while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
    UDR0 = (data >> 8); //send the higher bits
    while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
    //UDR0 = '\n';//send a new line just to be sure
}

void uart_sendstr(char *data) {
    /*
    Use this to send a string, it will split it up into individual parts
    send those parts, and then send the new line code
    */
    while (*data) {
        while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
        UDR0 = *data; //goes through and splits the string into individual bits, sends them
        data += 1;//go to new bit in string
    }
    while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
    //UDR0 = '\n';//send a new line just to be sure
}

void uart1_sendstr(char *data) {
    /*
    Use this to send a string, it will split it up into individual parts
    send those parts, and then send the new line code
    */
    while (*data) {
        while ((UCSR1A & (1 << UDRE1)) == 0);//make sure the data register is cleared
        UDR1 = *data; //goes through and splits the string into individual bits, sends them
        data += 1;//go to new bit in string
    }
    while ((UCSR1A & (1 << UDRE1)) == 0);//make sure the data register is cleared
    //UDR0 = '\n';//send a new line just to be sure
}

ISR(SIG_USART_RECV) {//sets up the interrupt to recieve any data coming in
        input_buffer[read_spot] = UDR0;
        read_spot++;//and "exports" if you will the data to a variable outside of the register
        //until the main program has time to read it. makes sure data isn't lost as much
        if(read_spot == BUFF_LEN) read_spot = 0;
}
