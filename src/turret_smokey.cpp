#include<avr/io.h>
#include "drive/turretdrv.h"


#define F_CPU 16000000l
#define BAUD 38400
#define BUFF_LEN 63
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)

#include <util/delay.h>

void tx_uart(char *data, int size)
{
    for(int i = 0; i < size; i++)
    {
        while(! (UCSR0A & (1 << UDRE0)));
        UDR0 = data[i];
    }
}

void init_uart()
{
    UCSR0B |= (1 << RXEN0);

    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

    UBRR0L = BAUD_PRESCALE; //set the baud to 9600, have to split it into the two registers
    UBRR0H = (BAUD_PRESCALE >> 8); //high end of baud register
}


// Words on pins 11 (x) and 12 (y)
int main (void) {
    init_turret_pwm();
    while (1)
      {

      set_turret_x_val(0); //leave servo at min rotation
      set_turret_y_val(0); //leave servo at min rotation
      _delay_ms(1000);
      set_turret_x_val(100); //leave serve at max rotation
      set_turret_y_val(100); //leave serve at max rotation
    _delay_ms(1000);

      }


}
