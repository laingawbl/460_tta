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

#include "drive/commdrv.h"
#include <string.h>

// Test parsing of various packets
int main()
{
    init_uart();
    
    // Convert structs to strings
    Base_To_Remote_Pkt_T base_to_remote;
    Remote_To_Base_Pkt_T remote_to_base;
    
    char *base_to_remote_string = NULL;
    char *remote_to_base_string = NULL;
    
    base_to_remote.laser_power = true;
    base_to_remote.turret_x_pos = 50;
    base_to_remote.turret_y_pos = -50;
    base_to_remote.rotate = 10;
    base_to_remote.direction = -100;
    
    remote_to_base.photoresistor_value = 1050;
    
    // Pretty print struct
    base_to_remote_string = pretty_print_base_to_remote_struct(&base_to_remote);
    tx_uart(base_to_remote_string, strlen(base_to_remote_string));
    free(base_to_remote_string);
    
    // Print struct as packetized string
    base_to_remote_string = base_to_remote_struct_to_string(&base_to_remote);
    tx_uart(base_to_remote_string, strlen(base_to_remote_string));
    
    tx_uart("\n-----\n", strlen("\n-----\n"));
    
    // Pretty print struct
    remote_to_base_string = pretty_print_remote_to_base_struct(&remote_to_base);
    tx_uart(remote_to_base_string, strlen(remote_to_base_string));
    free(remote_to_base_string);
    
    // Print struct as packetized string
    remote_to_base_string = remote_to_base_struct_to_string(&remote_to_base);
    tx_uart(remote_to_base_string, strlen(remote_to_base_string));
    
    tx_uart("\n-----\n", strlen("\n-----\n"));
    tx_uart("Values Below Should Match\n", strlen("Values Below Should Match\n"));
    
    // Convert strings back to structs, and print to make sure they work
    
    // Clear Structs
    memset(&base_to_remote, 0x00, sizeof(base_to_remote));
    memset(&remote_to_base, 0x00, sizeof(remote_to_base));
    
    // Parse strings
    remote_to_base_string_to_struct(remote_to_base_string, &remote_to_base);
    free(remote_to_base_string);
    
    // Pretty print struct - Should match
    remote_to_base_string = pretty_print_remote_to_base_struct(&remote_to_base);
    tx_uart(remote_to_base_string, strlen(remote_to_base_string));
    free(remote_to_base_string);
    
    // Parse strings
    base_to_remote_string_to_struct(base_to_remote_string, &base_to_remote);
    free(base_to_remote_string);
    
    // Pretty print struct - Should match
    base_to_remote_string = pretty_print_base_to_remote_struct(&base_to_remote);
    tx_uart(base_to_remote_string, strlen(base_to_remote_string));
    free(base_to_remote_string);
}
