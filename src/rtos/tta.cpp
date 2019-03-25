/*!
 \file tta.c
 \brief  Implementation of the time-triggered RTOS (as described in tta.h)
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "err.h"
#include "tta.h"

#ifdef __PROBE__
#include "probe.h"
#endif

#ifdef __UART__
#include "../uart/uart.h"
#endif

// Forward declaration for ISRs
void _osAbort(ECODE err);

// ======================
// KERNEL DATA STRUCTURES
// ======================

static volatile int32_t  seconds = 0;
static int32_t           jitters = 0;

static TaskBlock_t  Tasks[MAXTASKS];
static StateBlock_t States[MAXTASKS];
static TaskQueue_t  NCTaskQueue;

// ==========================
// INTERRUPT SERVICE ROUTINES
// ==========================

/**
 \brief Timing-panic watchdog interrupt

 The RTOS keeps track of time using the _osGetClock() method, which relies
 on both the seconds variable and the current value of TCNT5 (the counter for
 Timer 5). seconds should be updated by the TIMER5_COMPA ISR every second,
 but this is not a hard deadline. At 1024x prescaling, TCNT5 will overflow
 once every ~4.2 seconds, and so _osGetClock() can still get the correct time
 for up to 3.2 seconds by looking at TCNT5.

 However, if this has not happened by that time, the potential for serious
 timing errors arises. In particular, we run the risk of
 -# the TIMER5_COMPA interrupt queueing up, but never running;
 -# TCNT5 overflowing without seconds being updated; and
 -# the RTOS returning from a task, calling _osGetClock() to get the time, and
    suddenly thinking it has gone back in time by 1-3 seconds.
 
 Further, if the RTOS goes for 3 seconds without ever entering idle (and thus
 enabling interrupts), it is intuitively likely that something is not right.
 Note that this is a "post-facto" watchdog: it will only trigger and halt the
 OS when it enters idle (enabling interrupts), as opposed to triggering in the
 middle of an abnormally long-running task. It is diagnostic, not preventative!

 This safeguarding is the cost of treating ISRs like periodic tasks, and
 keeping interrupts disabled for extended periods, in order to gain absolute
 TTA timing stability.
*/
ISR(TIMER5_OVF_vect){
    _osAbort(E_TICK_UPDATE_WATCHDOG);
}

/**
 \brief Timekeeping interrupt

 As mentioned above, if the OS does not enter idle for more than 3 seconds, this
 ISR may fail to run, which will prompt the TIMER5_OVF interrupt and abort the
 RTOS at the next opportunity.
 */
ISR(TIMER5_COMPA_vect){
    seconds += (TCNT5 / ONE_SECOND_1024);
    TCNT5 %= ONE_SECOND_1024;
}

// ===============
// KERNEL ROUTINES
// ===============

/*!
 * \brief Get the current RTOS tick by looking at TCNT5
 * \return The current RTOS tick (default 500us, defined in tta.h) since MCU boot or reset.
 */
inline uint32_t _osGetTick(void){
    return (seconds * (FCPU / TICK)) + (TCNT5 / (TICK / 1024));
}

/**
 * \brief Halt the RTOS and report an error code over UART0.
 * \param err The error code, from the ECODE enum, to report.
 */
void _osAbort(ECODE err) {

#ifdef __PROBE__
    pinSet(PROBE_15_ABORT);
#endif

#ifdef __UART__
    Enable_Interrupt();
    uart_sendstr((char*)ECODENames[err]);
    Disable_Interrupt();
#endif
    for(;;);
}

/**
 * \brief Add a task to the FIFO queue of noncritical tasks.
 *
 * Warning!: This procedure will \b overwrite queued tasks (starting with the first
 * task in the queue) if called when the queue is full. It is (as usual) the user's
 * responsibility to make sure this does not happen.
 *
 * \param what The TaskHandle to add to the queue.
 */
void _osAddTaskQueue(TaskHandle what){
    NCTaskQueue.tasks[NCTaskQueue.tail] = what;
    NCTaskQueue.tail = (NCTaskQueue.tail + 1) % MAXTASKS;
}

/**
 * \brief Get the next noncritical task to be run, removing it from the FIFO queue.
 * \return A handle to said task, or nullptr if the queue is empty.
 */
TaskHandle _osYankTaskQueue(){
    if (NCTaskQueue.head == NCTaskQueue.tail)
        return nullptr;

    TaskHandle next = NCTaskQueue.tasks[NCTaskQueue.head];
    NCTaskQueue.head = (NCTaskQueue.head + 1) % MAXTASKS;
    return next;
}

/**
 * \brief Get a free (i.e., DEAD) task block.
 * \return The lowest-indexed DEAD task block, or null if none exists.
 */
TaskHandle _osGetFreeTaskBlock(){
    int i;
    for(i = 0; i < MAXTASKS; i++){
        if (Tasks[i].type == DEAD) {
            return &(Tasks[i]);
        }
    }
    return nullptr;
}

/**
 * \brief Get a free (i.e., 0 ref count) state block.
 * \return The lowest-indexed free state block, or null if none exists.
 */
StateHandle _osGetFreeStateBlock(){
    int i;
    for(i = 0; i < MAXTASKS; i++){
        if (States[i].refs == 0) {
            return &(States[i]);
        }
    }
    return nullptr;
}

/**
 * \brief Helper function to calculate a periodic task's next runtime.
 * \param timing The task's timing attribute.
 * \return The next tick on which the task should run.
 */
uint32_t _osGetNextRunTime(Timing_t timing){
    uint32_t now = _osGetTick();
    return now - (now % timing.period) + (timing.offset);
}

/**
 * \brief Init function to set up resources needed by the RTOS; in particular, to start Timer 5.
 */
void _osInit(void){
    // Interrupts are always off, EXCEPT in _osIdle()
    Disable_Interrupt();

    seconds = 0;
    jitters = 0;

    // Initialise Timer5 as a counter for timekeeping / watchdogging (see TIMER5 ISRs at the start of this file)

    TCCR5A = 0;
    TCCR5B = 0;               // Stop Timer 5
    OCR5A  = ONE_SECOND_1024; // 16M / 1024 = 15625
    TCNT5  = 0;
    TCCR5B |= (1 << CS52) | (1 << CS50);     //CTC mode, 1024x prescale
    TIMSK5 |= (1 << OCIE5A) | (1 << TOIE5);

#ifdef __UART__
    Enable_Interrupt();
    uart_sendstr((char*)"TCNT start\n");
    Disable_Interrupt();
#endif
}

/**
 \brief Determine the next time-critical task to be run, and check it for timing errors.
 \param now The current tick, as reported by _osGetTick().
 \return The TaskHandle of the next time-critical task to be run (either periodic or single).
*/
TaskHandle _osDispatch(int32_t now){

#ifdef __PROBE__
    pinSet(PROBE_5_DISPATCH);
#endif

#ifdef __VERBOSE_TIMING__
    char numString[7];
    utos(now, numString);
    Enable_Interrupt();
    uart_sendstr("\nnow:");
    uart_sendstr(numString);
    uart_sendstr("\t");
    Disable_Interrupt();
#endif

    TaskHandle next = nullptr;
    int32_t closest = INT32_MAX;
    
    // Select the task with the closest deadline (which may be in the past!)

    int i;
    for (i = 0; i < MAXTASKS; i++){
        if (Tasks[i].type == PERIODIC || Tasks[i].type == SINGLE) {
            int32_t time_until = (Tasks[i].nextRun - now);

#ifdef __VERBOSE_TIMING__
            utos(time_until, numString);
            Enable_Interrupt();
            uart_sendstr(numString);
            uart_sendstr("\t");
            Disable_Interrupt();
#endif
            if (time_until < closest) {
                closest = time_until;
                next = &(Tasks[i]);
            }
        }
    }

    // Now that we have the next task to run, examine it for timing errors.
    // If the deadline was in the past, either register jitter (if less than
    // MAXJITTER ticks prior), or abort with a timing error.

    if(closest + MINJITTER_TICKS <= 0) {
        if (closest + MAXJITTER_TICKS <= 0) {
            _osAbort(E_TASK_DEADLINE_MISSED);
        }
        else {
            jitters += 1;

#ifdef __VERBOSE_TIMING__
            utos(jitters, numString);
            Enable_Interrupt();
            uart_sendstr("\nJITTERS:");
            uart_sendstr(numString);
            uart_sendstr("\n");
            Disable_Interrupt();
#endif
            // Too many jittered tasks will also cause an abort with timing error.
            if (jitters > MAXJITTER_COUNT) {
                _osAbort(E_TOO_MANY_JITTERED_TASKS);
            }
        }
    }

    return next;
}

/**
 * \brief Idle until a certain time (in ticks). Noncritical tasks and ISRs will run during this time.
 *
 * Note that, even if called in the middle of a tick, this routine will exit about a microsecond after the
 * next tick rolls over, aligning the kernel's execution with the tick cycle.
 *
 * \param until The time (in ticks) to idle for.
 */
void _osIdle(int32_t until){

    // Now is the time to run ISRs
    Enable_Interrupt();

#ifdef __PROBE__
    pinSet(PROBE_2_IDLING);
#endif

    int i;
    while(_osGetTick() < until){

        // Now is also the time to run noncritical tasks, if there are any
        TaskHandle nextNC = _osYankTaskQueue();
        if(nextNC) {

#ifdef __PROBE__
            pinSet(PROBE_3_IN_NC_TASK);
#endif

            (nextNC->func)(&(nextNC->state->state));

#ifdef __PROBE__
            pinSet(PROBE_2_IDLING);
#endif

            OS_YankTask(nextNC);
        }
        // Three operations per loop: compare i, increment i, nop (... I think...)
        // So just FYI this will take about 12 microseconds
        for(i = 0; i < 64; i++)
            asm volatile ("nop");
    }

    Disable_Interrupt();
}

/**
 * \brief Helper function to perform task-setup operations common to all 3 task types.
 * \param func The function to be run during the task.
 * \param passedState The state block to call func with; passing a null pointer will
        cause the kernel to allocate a new state block for that task.
 * \return A handle to the (incomplete) task in the RTOS's task table.
 */
TaskHandle _osCommonTaskSetup(TaskFunction_t func, StateHandle passedState){
    // Before anything else, see if the RTOS has space for another task
    TaskHandle newTask = _osGetFreeTaskBlock();
    if (!newTask) {
        _osAbort(E_TOO_MANY_TASKS);
    }
    else {
        newTask->func = func;

        // Set up state block for the task
        // If one was provided (from another task), use it
        if (passedState) {
            OS_SetTaskState(newTask, passedState);
        }
            // Otherwise, attempt to acquire a free state block, aborting with a task error if we can't
        else {
            StateHandle newStateBlock = _osGetFreeStateBlock();
            if (!newStateBlock)
                _osAbort(E_TOO_MANY_STATES);
            else
                OS_SetTaskState(newTask, newStateBlock);
        }
    }
    return newTask;
}

// ============
// API ROUTINES
// ============

/// These are described in tta.h.

TaskHandle OS_CreateTask(TaskFunction_t func, StateHandle passedState, Timing_t timing) {
    TaskHandle newTask = _osCommonTaskSetup(func, passedState);
    newTask->type = PERIODIC;
    newTask->timing = timing;
    newTask->nextRun = _osGetNextRunTime(timing);

    return newTask;
}

TaskHandle OS_CreateSingleTask(TaskFunction_t func, StateHandle passedState, int32_t when){
    TaskHandle newTask = _osCommonTaskSetup(func, passedState);
    newTask->type = SINGLE;
    newTask->nextRun = _osGetTick() + when;

    return newTask;
}

TaskHandle OS_CreateNonCriticalTask(TaskFunction_t func, StateHandle passedState){
    TaskHandle newTask = _osCommonTaskSetup(func, passedState);
    newTask->type = NONCRITICAL;

    // Enqueue the new task in the noncritical FIFO queue
    _osAddTaskQueue(newTask);

    return newTask;
}

void OS_YankTask(TaskHandle task){
    task->type = DEAD;
    OS_SetTaskState(task, nullptr);
}

StateHandle OS_GetTaskState(TaskHandle task){
    return task->state;
}

void OS_SetTaskState(TaskHandle task, StateHandle newState){
    if(task->state) {
        task->state->refs -= 1;
    }

    task->state = newState;

    if(newState) {
        task->state->refs += 1;
    }
}

void OS_Run(void) {

#ifdef __PROBE__
    pinStart(PIN_10 | PIN_11 | PIN_12 | PIN_13);
    pinSet(PROBE_1_IN_KERNEL);
#endif

#ifdef __UART__
    uart_start();
#endif

    int32_t kern_tick;

    _osInit();

    for(;;){
        kern_tick = _osGetTick();

        TaskHandle next = _osDispatch(kern_tick);

#ifdef __PROBE__
        pinSet(PROBE_1_IN_KERNEL);
#endif

        // If the next task exists and has to be run soon, enter it by
        // calling its `func` member, with its `state` member as the
        // argument.
        if (next && (next->nextRun - kern_tick < 5)) {
            if (next->nextRun > kern_tick){
                _osIdle(next->nextRun);
            }
#ifdef __PROBE__
            pinSet(PROBE_4_IN_TIMED_TASK);
#endif

            (next->func)(&(next->state->state));

#ifdef __PROBE__
            pinSet(PROBE_1_IN_KERNEL);
#endif

            // Now we have returned from the task; if it was a one-time task, unschedule it
            if (next->type == SINGLE){
                OS_YankTask(next);
            }
            // Otherwise, the task is periodic; calculate its next runtime before we forget about it
            else if (next->type == PERIODIC) {
                next->nextRun += next->timing.period;
            }
            else {
                _osAbort(E_TOO_MANY_STATES);
            }
        }
        // We refrain from idling until the next task to check _osDispatch() regularly, in case noncritical tasks or
        // ISRs have added tasks during the last five ticks which are to run before (next).
        else {
            _osIdle(kern_tick + 5);

#ifdef __PROBE__
            pinSet(PROBE_1_IN_KERNEL);
#endif
        }
    }
}