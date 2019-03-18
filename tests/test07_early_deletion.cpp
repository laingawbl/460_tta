//
// Created by auriga on 11/03/19.
//

#include "../src/tta.h"

void uart_sendstr(char *str);

/*
 * Expected result: "once!" never prints
 */

struct dummy {
    TaskHandle singleTask;
};

void once(void *state){
    Enable_Interrupt();
    uart_sendstr((char*)"\tonce!\n");
    Disable_Interrupt();
}

void pong(void *state){

    TaskHandle theTask = ((struct dummy *)state)->singleTask;
    if(theTask)
        OS_YankTask(theTask);
}

void ping(void *state){

    ((struct dummy *)state)->singleTask = OS_CreateTask(once, nullptr, {2000, 2000});
}

int main(void){
    TaskHandle pingHandle = OS_CreateTask(ping, nullptr, {4000, 1000});

    // instead of a new block, use ping's existing block
    OS_CreateTask(pong, OS_GetTaskState(pingHandle), {4000, 2000});
    OS_Run();
}