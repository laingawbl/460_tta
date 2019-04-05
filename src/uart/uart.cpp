#include<avr/io.h>
#include<avr/interrupt.h>

#include "uart.h"

static volatile uint8_t uart_buffer[UART_BUFFER_SIZE];
static volatile uint8_t uart_buffer_index;

static volatile uint8_t uart1_buffer[UART_BUFFER_SIZE];
static volatile uint8_t uart1_buffer_index;

static volatile uint8_t uart2_buffer[UART_BUFFER_SIZE];
static volatile uint8_t uart2_buffer_index;


//Got through and set up the registers for UART
void uart_start(UART_BPS bitrate) {
        UCSR0A = _BV(U2X0);
        UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
        UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

        UBRR0H = 0;	// for any speed >= 9600 bps, the UBBR value fits in the low byte.

        // See the appropriate AVR hardware specification for a table of UBBR values at different
        // clock speeds.
        switch (bitrate) {
            case UART_9600:
                UBRR0L = 208;
                break;
            case UART_19200:
                UBRR0L = 103;
                break;
            case UART_38400:
                UBRR0L = 51;
                break;
            case UART_57600:
                UBRR0L = 34;
                break;
            default:
                UBRR0L = 103;
        }
}

void uart1_start(UART_BPS bitrate) {
    UCSR1A = _BV(U2X1);
    UCSR1B = _BV(RXEN1) | _BV(TXEN1) | _BV(RXCIE1);
    UCSR1C = _BV(UCSZ11) | _BV(UCSZ10);

    UBRR1H = 0;	// for any speed >= 9600 bps, the UBBR value fits in the low byte.

    // See the appropriate AVR hardware specification for a table of UBBR values at different
    // clock speeds.
    switch (bitrate) {
        case UART_9600:
            UBRR1L = 208;
            break;
        case UART_19200:
            UBRR1L = 103;
            break;
        case UART_38400:
            UBRR1L = 51;
            break;
        case UART_57600:
            UBRR1L = 34;
            break;
        default:
            UBRR1L = 103;
    }
}

void uart2_start(UART_BPS bitrate) {
    UCSR2A = _BV(U2X2);
    UCSR2B = _BV(RXEN2) | _BV(TXEN2) | _BV(RXCIE2);
    UCSR2C = _BV(UCSZ21) | _BV(UCSZ20);

    UBRR2H = 0;	// for any speed >= 9600 bps, the UBBR value fits in the low byte.

    // See the appropriate AVR hardware specification for a table of UBBR values at different
    // clock speeds.
    switch (bitrate) {
        case UART_9600:
            UBRR2L = 208;
            break;
        case UART_19200:
            UBRR2L = 103;
            break;
        case UART_38400:
            UBRR2L = 51;
            break;
        case UART_57600:
            UBRR2L = 34;
            break;
        default:
            UBRR2L = 103;
    }
}

void uart_sendchar(char data){
    while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
    UDR0 = data; //goes through and splits the string into individual bits, sends them
}

void uart1_sendchar(char data){
    while ((UCSR1A & (1 << UDRE1)) == 0);
    UDR1 = data;
}

void uart2_sendchar(char data){
    while ((UCSR2A & (1 << UDRE2)) == 0);
    UDR2 = data;
}

void uart_sendstr(char *data) {
    /*
    Use this to send a string, it will split it up into individual parts
    send those parts, and then send the new line code
    */
    while (*data) {
        uart_sendchar(*data);
        data += 1;//go to new bit in string
    }
    while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
}

void uart1_sendstr(char *data) {
    /*
    Use this to send a string, it will split it up into individual parts
    send those parts, and then send the new line code
    */
    while (*data) {
        uart1_sendchar(*data);
        data += 1;//go to new bit in string
    }
    while ((UCSR1A & (1 << UDRE1)) == 0);//make sure the data register is cleared
}

void uart2_sendstr(char *data) {
    /*
    Use this to send a string, it will split it up into individual parts
    send those parts, and then send the new line code
    */
    while (*data) {
        uart2_sendchar(*data);
        data += 1;//go to new bit in string
    }
    while ((UCSR2A & (1 << UDRE2)) == 0);//make sure the data register is cleared
}

/**
 * Receive a single byte from the receive buffer
 *
 * @param index
 *
 * @return
 */
uint8_t uart_get_byte(int index)
{
    if (index < UART_BUFFER_SIZE)
    {
        return uart_buffer[index];
    }
    return 0;
}

/**
 * Receive a single byte from the receive buffer
 *
 * @param index
 *
 * @return
 */
uint8_t uart1_get_byte(int index)
{
    if (index < UART_BUFFER_SIZE)
    {
        return uart1_buffer[index];
    }
    return 0;
}

uint8_t uart2_get_byte(int index)
{
    if (index < UART_BUFFER_SIZE)
    {
        return uart2_buffer[index];
    }
    return 0;
}

/**
 * Get the number of bytes received on UART
 *
 * @return number of bytes received on UART
 */
uint8_t uart_bytes_received(void)
{
    return uart_buffer_index;
}

/**
 * Get the number of bytes received on UART
 *
 * @return number of bytes received on UART
 */
uint8_t uart1_bytes_received(void)
{
    return uart1_buffer_index;
}

uint8_t uart2_bytes_received(void)
{
    return uart2_buffer_index;
}

/**
 * Prepares UART to receive another payload
 *
 */
void uart_reset_receive(void)
{
    uart_buffer_index = 0;
}

/**
 * Prepares UART to receive another payload
 *
 */
void uart1_reset_receive(void)
{
    uart1_buffer_index = 0;
}

/**
 * Prepares UART to receive another payload
 *
 */
void uart2_reset_receive(void)
{
    uart2_buffer_index = 0;
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

ISR(USART0_RX_vect){
    while(!(UCSR0A & (1<<RXC0)));
    uart_buffer[uart_buffer_index] = UDR0;
    uart_buffer_index = (uart_buffer_index + 1) % UART_BUFFER_SIZE;
}

ISR(USART1_RX_vect){
    while(!(UCSR1A & (1<<RXC1)));
    uart1_buffer[uart1_buffer_index] = UDR1;
    uart1_buffer_index = (uart1_buffer_index + 1) % UART_BUFFER_SIZE;
}

ISR(USART2_RX_vect){
        while(!(UCSR2A & (1<<RXC2)));
        uart2_buffer[uart2_buffer_index] = UDR2;
        uart2_buffer_index = (uart2_buffer_index + 1) % UART_BUFFER_SIZE;
}
