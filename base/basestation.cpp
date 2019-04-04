// 460 Project 1 Phase 2 - HOST
// The host (base station) has the joystick, photocell, and LCD.

#include <LiquidCrystal.h>

//////////
// defines
//////////

// scheduler defines

#define SCHED_MAXTASKS 8

typedef void (*task_cb)();

typedef struct
{
    int32_t period;
    int32_t remaining_time;
    uint8_t is_running;
    task_cb callback;
} task_t;

task_t tasks[SCHED_MAXTASKS];

uint32_t last_runtime;

// scheduling periods, in milliseconds

#define JOY_PERIOD 50     // 20Hz input frequency
#define PHOTO_PERIOD 100  // photocell time constants are on the order of 100ms - but task for reading it is tiny
#define LCD_PERIOD 100
#define BT_PERIOD 50

// inputs`

//#define LCD_BTN A0      // A0 is occluded by the DFRobot LCD shield
#define JOY_X A8
#define JOY_Y A9
#define JOY_PUSH 42

#define PHOTO A10

#define BT_BAUD 9600

#define SCHED_IDLE_OUT 13

int joyNeutralX, joyNeutralY; // initial stick readings (assumed to be neutral)
float joySmoothX, joySmoothY;   // smoothed output values
int joyPush;                  // joystick pushbutton value
// even for military-grade lasing purposes, no debouncing is needed

int outX = 0;
int outY = 0;

int JOY_DZ_X = 100;            // deadzone band widths
int JOY_DZ_Y = 100;
float JOY_ALPHA = 0.1;          // exponential-average filter constant

int photoHit = 0;             // is the photocell hit?
int PHOTO_TRIGGER = 150;      // minimum level (between 0-1024) photocell reading to consider a "hit"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // written for the DFRobot LCD KeyPad shield

//////////////////
// setup functions
//////////////////

void
photo_setup(){
    pinMode(PHOTO, INPUT); // ATTENTION! the photocell requires a ~10k pulldown resistor between its output side and GND.
}

void
joy_setup()
{
    pinMode(JOY_X, INPUT);
    pinMode(JOY_Y, INPUT);
    pinMode(JOY_PUSH, INPUT_PULLUP);

    joyNeutralX = analogRead(JOY_X);  // obtain what we assume is stick neutral position (READ: don't touch the stick when pressing RESET)
    joyNeutralY = analogRead(JOY_Y);

    joySmoothX = joyNeutralX;
    joySmoothY = joyNeutralY;
}

void
lcd_setup()
{
    lcd.begin(16, 2);
}

void
bt_setup()
{
    Serial1.begin(BT_BAUD);
}

void
sched_setup()
{
    pinMode(SCHED_IDLE_OUT, OUTPUT);

    last_runtime = millis();
}

//////////////////////
// scheduler functions
// this is nrqm's code
//////////////////////

void
sched_start_task(int16_t wait, int16_t period, task_cb task)
{
    static uint8_t id = 0;
    if (id < SCHED_MAXTASKS)
    {
        tasks[id].remaining_time = wait;
        tasks[id].period = period;
        tasks[id].is_running = 1;
        tasks[id].callback = task;
        id++;
    }
}

void
idle(uint32_t idlePeriod)
{
    digitalWrite(SCHED_IDLE_OUT, HIGH);
    delay(idlePeriod);
    digitalWrite(SCHED_IDLE_OUT, LOW);
}

uint32_t
sched_dispatch()
{
    uint8_t i;
    uint32_t now = millis();
    uint32_t elapsed = now - last_runtime;
    last_runtime = now;
    task_cb t = NULL;
    uint32_t idle_time = 0xFFFFFFFF;

    for (i = 0; i < SCHED_MAXTASKS; i++)                  // update each task's remaining time, and identify the first ready task (if there is one).
    {
        if (tasks[i].is_running)
        {
            tasks[i].remaining_time -= elapsed;               // update the task's remaining time
            if (tasks[i].remaining_time <= 0)
            {
                if (t == NULL)                                  // if this task is ready to run, and we haven't already selected a task to run, select this one.
                {
                    t = tasks[i].callback;
                    tasks[i].remaining_time += tasks[i].period;
                }
                idle_time = 0;
            }
            else
                idle_time = min((uint32_t)tasks[i].remaining_time, idle_time);
        }
    }

    if (t != NULL)                                        // If a task was selected to run, call its function.
        t();

    return idle_time;
}

////////////
// loop tasks
/////////////

// Read joystick values and calculate smoothed input values, updating joySmoothX, joySmoothY, and joyPush

void
joy_read()
{

    joyPush = ! (digitalRead (JOY_PUSH));             // as mentioned above, no debouncing needed

    int joyRawY = analogRead (JOY_X);
    int joyRawX = analogRead (JOY_Y);

    if(abs(joyRawX - joyNeutralX) > JOY_DZ_X) {
        joySmoothX = (joySmoothX * (1.0 - JOY_ALPHA)) + (joyRawX * JOY_ALPHA);
    }
    else {
        joySmoothX = joyNeutralX;
    }

    if(abs(joyRawY - joyNeutralY) > JOY_DZ_Y) {
        joySmoothY = (joySmoothY * (1.0 - JOY_ALPHA)) + (joyRawY * JOY_ALPHA);
    }
    else {
        joySmoothY = joyNeutralY;
    }

    if (joySmoothX >= joyNeutralX)
        outX = map(joySmoothX, joyNeutralX, 1024, 0, 400);
    else
        outX = map(joySmoothX, 0, joyNeutralX, -400, 0);

    if (joySmoothY >= joyNeutralY)
        outY = map(joySmoothY, joyNeutralY, 1024, 0, 400);
    else
        outY = map(joySmoothY, 0, joyNeutralY, -400, 0);

}

// Read photocell value and set photoHit accordingly

void photo_read()
{
    photoHit = (analogRead(PHOTO) > PHOTO_TRIGGER);
}

void
lcd_drive(){
    lcd.setCursor(0,0);     // beginning of 1st line
    if(joyPush)
        lcd.print("L: FIRE");
    else
        lcd.print("L: OFF ");
    lcd.setCursor(8,0);     // beginning of 2nd line
    if(photoHit)
        lcd.print("P: HIT!");
    else
        lcd.print("P: MISS");
}


void
bt_trans()
{
    // at 960 cps, an 8-byte message should take at least 8.3 milliseconds to transmit.

    Serial1.write(0x21);               // '!'
    Serial1.write((outX >> 8) & 0xFF); // high byte
    Serial1.write(outX & 0xFF);        // low byte
    Serial1.write(0x2F);               // '/'
    Serial1.write((outY >> 8) & 0xFF); // high byte
    Serial1.write(outY & 0xFF);        // low byte
    Serial1.write(0x2F);               // '/'
    Serial1.write(joyPush & 0xFF);
}

/////////////////////
// setup() and loop()
/////////////////////

void
setup()
{
    photo_setup();
    sched_setup();
    joy_setup();

    sched_start_task(0, JOY_PERIOD, joy_read);
    sched_start_task(5, PHOTO_PERIOD, photo_read);
    sched_start_task(20, BT_PERIOD, bt_trans);
    sched_start_task(70, LCD_PERIOD, lcd_drive);    // 20 + 50, will alternate with bt_trans
}

void
loop()
{
    uint32_t idle_time = sched_dispatch();
    if (idle_time)
        idle(idle_time);
}