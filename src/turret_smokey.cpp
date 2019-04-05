#include<avr/io.h>


#define F_CPU 16000000l
#define BAUD 38400
#define BUFF_LEN 63
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)

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

#include<avr/io.h>
#include <util/delay.h>


int main (void) {
  //Set testFactor to 1 for real life or to a higher value
  //to speed up the simulation
  int testFactor=1; //ICR1 and OCR1A are divided by this value


  //Initialize PORTB

  PORTB=0b00100000;  //start with OC1A high
  DDRB= 0xFF;  //set OC1A to output (among others)

  ICR1=20000/testFactor;
  OCR1A=1000/testFactor; //set 1ms pulse  1000=1ms  2000=2ms
  TCCR1A=(1<<COM1A1);//COM1A1 Clear OCnA when match counting up,Set on 

  TCCR1B=(1<<WGM13)|(1<<CS11);// Phase and Freq correct ICR1=Top

while (1)
      {

      OCR1A = 1000; //leave servo at min rotation
      _delay_ms(1000);
      OCR1A = 2000; //leave serve at max rotation
    _delay_ms(1000);

      };


}
