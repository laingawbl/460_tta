
#include "autoctl.h"

#define MANUAL_BACKOFF 16;
#define AUTO_BACKOFF 2;

static TaskHandle autoctlHandle = nullptr;

typedef enum {
    NOTHING = 0,
    TURN_LEFT,
    TURN_RIGHT,
    DRIVE_STRAIGHT,
    DRIVE_BACK
} Behaviour;

typedef enum {
    CLEAR,
    HIT_OBSTACLE,
    AVOIDING1,
    AVOIDING2
} avoidStateEnum;

typedef enum {
    MANUAL,
    AUTO
} Mode;

static avoidStateEnum avoidState = CLEAR;

static int backoffCounter = MANUAL_BACKOFF;
static Mode ctlMode = MANUAL;

void executeBehaviour(Behaviour what){
    switch(what){
        case TURN_LEFT:
            Roomba_Drive(1000, -1);
            break;
        case TURN_RIGHT:
            Roomba_Drive(1000, 1);
            break;
        case DRIVE_STRAIGHT:
            Roomba_Drive(200, 0x8000);
            break;
        case DRIVE_BACK:
            Roomba_Drive(-200, 0x8000);
            break;
        case NOTHING:
        default:
            Roomba_Drive(0, 0);
    }
}

Behaviour avoidObstacles(bool irHit, bool bumperHit){
    bool hitAny = (irHit || bumperHit);
    switch(avoidState){
        case CLEAR:
            if(hitAny){
                avoidState = HIT_OBSTACLE;
                return DRIVE_BACK;
            }
            else {
                return NOTHING;
            }

        case HIT_OBSTACLE:
            if(! bumperHit){
                avoidState = AVOIDING1;
                return TURN_LEFT;
            }
            return DRIVE_BACK;

        case AVOIDING1:
            if(irHit){
                return TURN_LEFT;
            }
            else if (bumperHit){
                avoidState = HIT_OBSTACLE;
                return DRIVE_BACK;
            }
            else {
                avoidState = AVOIDING2;
                return TURN_LEFT;
            }

        case AVOIDING2:
            if(irHit){
                return TURN_LEFT;
            }
            else if (bumperHit){
                avoidState = HIT_OBSTACLE;
                return DRIVE_BACK;
            }
            else {
                avoidState = CLEAR;
                return NOTHING;
            }

        default:
            avoidState = HIT_OBSTACLE;
            return DRIVE_BACK;
    }
}

void autoctlTask(void *){

    bool irHit = readIR();
    if(irHit){
        uart_sendstr("\tAUTOCTL: got IR\n");
    }
    bool bumperHit = readBumper();
    if(irHit){
        uart_sendstr("\tAUTOCTL: got bumper\n");
    }
    Behaviour nextAction = avoidObstacles(irHit, bumperHit);

    if(ctlMode == MANUAL){
        // override the user if we are close to death
        if(nextAction != NOTHING){
            uart_sendstr("got an emergency action!: switching to AUTO\n");
            ctlMode = AUTO;
            backoffCounter = AUTO_BACKOFF;
            executeBehaviour(nextAction);
        }
        // execute the user's packet if a nonzero packet was sent
        else if(haveManualControl()){
            backoffCounter = MANUAL_BACKOFF;
            Roomba_Drive(readManualV(), readManualR());
        }
        // hold still and decrement the backoff counter until we enter AUTO mode
        else {
            Roomba_Drive(0,0);
            backoffCounter--;
            if(backoffCounter == 0){
                uart_sendstr("switching to AUTO\n");
                ctlMode = AUTO;
                backoffCounter = AUTO_BACKOFF;
            }
        }
    }
    else {     // ctlMode = AUTO
        if(haveManualControl()){
            backoffCounter--;
            if(backoffCounter == 0){
                uart_sendstr("switching to MANUAL\n");
                ctlMode = MANUAL;
                backoffCounter = MANUAL_BACKOFF;
            }
        }
        else {
            backoffCounter = AUTO_BACKOFF;
            if(nextAction == NOTHING){
                uart_sendstr("driving straight\n");
                executeBehaviour(DRIVE_STRAIGHT);
            }
            else{
                executeBehaviour(nextAction);
            }

        }
    }
}

void autoctlStart(Timing_t when){
    Roomba_Init();

    if(!autoctlHandle){
        autoctlHandle = OS_CreateTask(autoctlTask, nullptr, when);
    }
}