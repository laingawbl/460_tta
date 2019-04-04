/**
 * @file basestation.cpp
 * @brief Basestation for controlling the Roomba for Project 3
 */

#include <avr/io.h>

#include "../src/lib/mcu.h"
#include "../src/uart/uart.h"
#include "../src/drive/commdrv.h"

#include <util/delay.h>

#define JOY_X PORTF0
#define JOY_Y PORTF1
#define MOV_X PORTF7
#define MOV_Y PORTF6
#define JOY_DDR DDRF
#define JOY_PIN PINF

#define JOY_PUSH PORTA0
#define MOV_PUSH PORTA1
#define JOY_PUSH_DDR DDRA
#define JOY_PUSH_PIN PINA
#define JOY_PUSH_PORT PORTA // for internal pullups

int joyNeutralX, joyNeutralY; // initial stick readings (assumed to be neutral)
int movNeutralX, movNeutralY;
float joySmoothX, joySmoothY;   // smoothed output values
float movSmoothX, movSmoothY;
int joyPush, movPush;           // joystick pushbutton values
// even for military-grade lasing purposes, no debouncing is needed

int outX = 0;
int outY = 0;
int outV = 0;
int outR = 0;

int JOY_DZ = 100;            // deadzone band widths
float JOY_ALPHA = 0.1;          // exponential-average filter constant

/*
 * Arduino dropins
 */

int analogRead(int channel){
    uint8_t high, low;
    int result;

    // 128x prescale clock, we are in no rush, and enable ADC
    ADCSRA |= ( 1 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 ) | (1 << ADEN);

    // select channel (mod 8, as high bit is handled by MUX5)
    ADMUX &= ~(0x07);
    ADMUX |= (channel & 0x07);

    // start comparison by setting ADSC
    ADCSRA |= (1 << ADSC);

    // wait for ADSC to clear on conversion finish
    while(ADCSRA & (1 << ADSC));

    low = ADCL;
    high = ADCH;
    result = (high << 8) | low;

    return result;
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    // "For the mathematically inclined, here’s the whole function" - the Arduino Reference website
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

long abs(long x){
    return (x > 0) ? x : (-1*x);
}

/*
 * setup functions
/*/

void
joy_setup()
{
    JOY_DDR &= ~((1 << JOY_X) | (1 << JOY_Y) | (1 << MOV_X) | (1 << MOV_Y));
    JOY_PUSH_DDR &= ~((1 << JOY_PUSH) | (1 << MOV_PUSH));
    JOY_PUSH_PORT |= (1 << JOY_PUSH) | (1 << MOV_PUSH);

    // use Vcc as reference
    ADMUX = (1 << REFS0);

    joyNeutralX = analogRead(JOY_X);  // obtain what we assume is stick neutral position (READ: don't touch the stick when pressing RESET)
    joyNeutralY = analogRead(JOY_Y);

    movNeutralX = analogRead(MOV_X);
    movNeutralY = analogRead(MOV_Y);

    joySmoothX = joyNeutralX;
    joySmoothY = joyNeutralY;

    movSmoothX = movNeutralX;
    movSmoothY = movNeutralY;
}
void
bt_setup()
{
    uart1_start(UART_9600);
    //uart_start(UART_38400);
}

/*
 * loop tasks
 */

// Read joystick values and calculate smoothed input values, updating joySmoothX, joySmoothY, and joyPush

float smooth(float oldSmooth, int raw, int neutral){
    float newSmooth = 0;
    if(abs(raw - neutral) > JOY_DZ) {
        newSmooth = (oldSmooth * (1.0 - JOY_ALPHA)) + (raw * JOY_ALPHA);
    }
    else {
        newSmooth = neutral;
    }
    return newSmooth;
}

long twoSidedMap(float value, int mid, int low, int high) {
    if (value >= mid) {
        return map((long) value, mid, 1024, 0, high);
    }
    else {
        return map((long) value, 0, mid, low, 0);
    }
}

void
joy_read()
{
    joyPush = ! (JOY_PUSH_PIN & (1 << JOY_PUSH));  // as mentioned above, no debouncing needed
    movPush = ! (JOY_PUSH_PIN & (1 << MOV_PUSH));

    int joyRawX = analogRead (JOY_X);
    int joyRawY = analogRead (JOY_Y);

    int movRawX = analogRead (MOV_X);
    int movRawY = analogRead (MOV_Y);

    joySmoothX = smooth(joySmoothX, joyRawX, joyNeutralX);
    joySmoothY = smooth(joySmoothY, joyRawY, joyNeutralY);
    outX = twoSidedMap(joySmoothX, joyNeutralX, -400, 400);
    outY = twoSidedMap(joySmoothY, joyNeutralX, -400, 400);

    movSmoothX = smooth(movSmoothX, movRawX, movNeutralX);
    movSmoothY = smooth(movSmoothY, movRawY, movNeutralY);
    outR = twoSidedMap(movSmoothX, movNeutralX, -500, 500);
    outV = twoSidedMap(movSmoothY, movNeutralY, -2000, 2000);

}

void
bt_trans()
{
    // set up the packet
    Base_To_Remote_Pkt_T packet = {
        (movPush && joyPush),
        outX,
        outY,
        outR,
        outV
    };

    // convert it to a string
    char * packetString = base_to_remote_struct_to_string(&packet);

    //char * prettyPacket = pretty_print_base_to_remote_struct(&packet);

    uart1_sendstr(packetString);
    //uart_sendstr(prettyPacket);
    //uart_sendchar('\n');

    free(packetString);
    //free(prettyPacket);
}

int
main()
{

    joy_setup();
    bt_setup();

    // HAHAHA! where is your "RTOS" now, Dr. Cheng?
    for(;;){
        joy_read();
        _delay_ms(25);
        bt_trans();
        _delay_ms(25);
    }

    return 0;
}