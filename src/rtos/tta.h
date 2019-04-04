#ifndef __TTA_H__
#define __TTA_H__

/*!
 \file tta.h
 \brief The interface for the time-triggered RTOS

 \mainpage Improved Time-Triggered RTOS
 This project builds upon the bare TTA scheduler seen in the first CSC460
 project. It adds support for calling tasks with persistent state parameters,
 dynamic creation and deletion of tasks, and support for one-shot tasks which
 can run at either a fixed delay from creation, or at the first suitable idle
 period in a first-come first-serve queue.

 \section assumptions Assumptions
 The RTOS has been written for the ATMega2560, and uses Timer5 for keeping
 track of time (leaving lower, more popular timers for other code).

 In spite of the improvements over a bare TTA scheduler, effort has been made
 to keep the RTOS as simple as possible. To simplify implementation, a wide
 range of responsibilities have been left to the user:
 -# That time-critical tasks do not overlap with each other;
 -# That time-critical tasks are not so close together that the kernel cannot
    run without jittering the start of the next task;
 -# That ISRs and noncritical tasks are short and infrequent enough to
    prevent task jitter (beyond user tolerance);
 -# That ISRs and noncritical tasks do not work with hard deadlines;
 -# That tasks properly use shared state; and
 -# That tasks do not write over unrelated kernel structures (i.e., there is
 \b no kernel memory protection!)

 Failing to achieve any of these conditions may cause the RTOS to abort with
 an error code (see \b _osAbort). The timing requirements may be loosened or
 sharpened by modifying MAXJITTER_TICKS and MAXJITTER_COUNT in this file.

 \section context Context Management
 This project, like the bare TTA it extends, chooses a "shared" approach to
 kernel/task memory allocation: that is, tasks run on the kernel's stack.
 The \b atomic \b nature of tasks in the TTA allows for this choice: as every
 task runs to completion without preemption, its context and stack do not
 need to be saved at any point. As we will discuss below, persistence of
 information between instances of a task, and between different tasks, is
 achieved through an arbitrary state parameter, instead.

 \section state Task State
 In the original TTA scheduler, all tasks must run as void functions; they
 can neither take parameters nor store information, except through global
 variables. This makes it awkward both to maintain information between runs of
 a task, and to run the same task using different parameters.

 The motivation for maintaining state is thus twofold:
 -# To allow tasks to pass state to later instances of themselves, and to and
    from other tasks; and
 -# To allow multiple instances of the same task with different parameters to
    run, allowing task code to be generalised.

 In this project, all user-defined tasks must be instances of

    void f( State_t *state )

 Where the parameter \b *state is a pointer to a state array of fixed size,
 and which may contain arbitrary information. It is expected that most task 
 functions will perform a cast from State_t to some task-specific struct
 with e.g.

    ...
    typedef struct {...} myStruct_t
    ...
    myStruct_t workspace = (myStruct_t *) state
    ...

 When  a task is created, it will be assigned a state block with which the
 kernel  will invoke it on all subsequent runs. This may either be a new empty
 block, or the state block of some other task, allowing for shared state
 between  tasks - as long as both tasks agree on how to interpret their shared
 block.

 Again, because of the \b atomic \b nature of TTA tasks, two tasks will never
 try to simultaneously access a shared memory resource, as two tasks cannot
 run at once (leaving aside ISRs).

 \author Alex Laing
 \author Casey Matson
*/

#include <stdint.h>
#include "../lib/mcu.h"

// =======
// DEFINES
// =======

// Tick length, in microseconds.
// Stability of the RTOS below 500us ticks has not been tested, but would allow for sharper timing abilities.
#define TICK_US 1000

// Clock cycles in a tick (e.g. 1ms = 16 kcycles at 16MHz).
#define TICK (( F_CPU / 1000000UL ) * TICK_US )

#define MAXTASKS 8

// Size of the persistent state blocks allocated to each task, in bytes.
#define STATESIZE 32

// Threshold between normal operation and jitter, in ticks.
#define MINJITTER_TICKS 4

// Threshold between mere jitter and a timing error, in ticks.
#define MAXJITTER_TICKS 40

// Number of jitter "incidents" to allow before aborting with a systematic jitter error.
#define MAXJITTER_COUNT 10

// =======================
// INSTRUMENTATION DEFINES
// =======================

// output RTOS state on digital pins 10-13 (see probe.h for state definitions)
//#define __PROBE__

// output tasking / error information on UART0 (see uart.h); needed for files in /test
#define __UART__

// timing information (requires __UART__ as well)
//#define __VERBOSE_TIMING__

// =====
// TYPES
// =====

/** A persistent state block. */
typedef char State_t[STATESIZE];

/** A pointer to a task's function. */
typedef void (*TaskFunction_t)(void *);

/**
 An enumeration of the states a task block can be in:
 -# DEAD: Yanked from scheduling.
 -# PERIODIC: Normal periodic task.
 -# SINGLE: One-off but time-critical task.
 -# NONCRITICAL: One-off task which can run whenever possible.
 */
typedef enum {
    DEAD = 0,
    PERIODIC,
    SINGLE,
    NONCRITICAL
} TASKTYPE;

typedef struct {
    int32_t period;
    int32_t offset;
} Timing_t;

typedef struct {
    State_t state;
    int refs = 0;
} StateBlock_t;

typedef StateBlock_t *StateHandle;

typedef struct {
    TASKTYPE       type = DEAD;
    TaskFunction_t func;
    StateHandle    state = nullptr;
    Timing_t       timing;
    int32_t       nextRun;
} TaskBlock_t;

typedef TaskBlock_t *TaskHandle;

typedef struct {
    TaskHandle tasks[MAXTASKS];
    int head = 0;
    int tail = 0;
} TaskQueue_t;

// =================
// TASK CREATION API
// =================

/**
 * \brief Create a normal periodic task.
 * \param func The function to be run during the task.
 * \param passedState The state block to call func with; passing a null pointer will
          cause the kernel to allocate a new state block for that task.
 * \param timing The period and offset (in ticks, measured from tick 0) at which
 *        the task should run.
 * \return A handle to the new task in the RTOS's task table.
 */

TaskHandle OS_CreateTask(TaskFunction_t func,
                         StateHandle passedState,
                         Timing_t timing);

/**
 \brief Create a new one-shot time-critical task.
 These tasks are run exactly once, and then removed from scheduling upon returning
 using OS_YankTask().
 \param func The function to be run during the task.
 \param passedState The state block to call func with; passing a null pointer will
        cause the kernel to allocate a new state block for that task.
 \param when The time (in ticks) at which the task should run.
 \return A handle to the new task in the RTOS's task table.
*/

TaskHandle OS_CreateSingleTask(TaskFunction_t func,
                               StateHandle passedState,
                               int32_t when);
/**
 \brief Create a new non-critical task.

 These tasks are placed into a FIFO queue, and run whenever the OS enters idle.
 Noncritical tasks may repeat themselves by calling 
    OS_CreateNonCriticalTask(OS_GetOwnBlock().func, state)
 \param func The function to be run during the task.
 \param passedState The state block to call func with; passing a null pointer will
        cause the kernel to allocate a new state block for that task.
 \return A handle to the new task in the RTOS's task table.
*/
TaskHandle OS_CreateNonCriticalTask(TaskFunction_t func,
                                    StateHandle passedState);
/** 
 \brief Remove a task from scheduling.
 The RTOS automatically calls this on one-time time-critical tasks when they return
 from execution.
 \param task The task to be removed.
 */
void OS_YankTask(TaskHandle task);

// =========
// STATE API
// =========

/**
 * \brief Get a task's state block.
 * \param task The task to get the state block for.
 * \return A handle for that task's state block.
 */
StateHandle OS_GetTaskState(TaskHandle task);

/**
 * \brief Switch a task's state block to something else, updating reference counts for the old and new blocks.
 *
 * Directly swapping blocks by modifying the StateHandle's state attribute may lead to "zombie" state blocks, which are
 * unused, but have a nonzero ref count and thus cannot be reused. Please always use this method instead.
 *
 * \param task The task to update the state block for.
 * \param newState A handle to the new state block for the task.
 */
void OS_SetTaskState(TaskHandle task, StateHandle newState);

// =================
// KERNEL PROCEDURES
// =================

/**
 \brief The main kernel loop.
 ISRs are disabled whenever a task is called.
*/
void OS_Run();

#endif
