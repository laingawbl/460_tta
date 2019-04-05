
#include "autoctl.h"

static TaskHandle manctlHandle = nullptr;

typedef enum {
    NOTHING = 0
    TURN_LEFT,
    TURN_RIGHT,
    DRIVE_STRAIGHT,
    DRIVE_BACK,
} Behaviour;

typedef enum {
    CLEAR,
    HIT_OBSTACLE,
    AVOIDING1,
    AVOIDING2
} avoidStateEnum;

static avoidStateEnum avoidState = CLEAR;

void executeBehaviour(Behaviour what){
    switch(what){
        case TURN_LEFT:
            Roomba_Drive(1000, -1);
            break;
        case TURN_RIGHT:
            Roomba_Drive(1000, 1);
            break;
        case DRIVE_STRAIGHT:
            Roomba_Drive(300, 0x8000);
            break;
        case DRIVE_BACK:
            Roomba_Drive(-300, 0x8000);
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
                return DRIVE_STRAIGHT;
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
                return DRIVE_STRAIGHT;
            }

        default:
            avoidState = HIT_OBSTACLE;
            return DRIVE_BACK;
    }
}

void autoctlTask(void *){
    if(haveManualControl()){
        Roomba_Drive(readManualV(), readManualR());
    }
    else {
        bool irHit = readIR();
        bool bumperHit = readBumper();
        Behaviour nextAction;
        nextAction = avoidObstacles(irHit, bumperHit);

        executeBehaviour(nextAction);
    }
}

void autoctlStart(Timing_t when){
    Roomba_Init();

    if(!autoctlHandle){
        autoctlHandle = OS_CreateTask(autoctlTask, nullptr, when);
    }
}