#include<avr/io.h>

 #define F_CPU 16000000l
 #define BAUD 38400
 #define BUFF_LEN 63
 #define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)

int main() {
    char thingy = '0';
    int i;
    
    UCSR0B |= (1 << RXEN0);

    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

    UBRR0L = BAUD_PRESCALE; //set the baud to 9600, have to split it into the two registers
    UBRR0H = (BAUD_PRESCALE >> 8); //high end of baud register

    for(;;){
        thingy = '0';
        for(i = 0; i < 36; i++){
            while(! (UCSR0A & (1 << UDRE0)));
            UDR0 = thingy;
            thingy++;
        }
    }
    return 0;
}
