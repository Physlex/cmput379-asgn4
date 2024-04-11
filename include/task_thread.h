#ifndef INCLUDE_TASK_THREAD_H
#define INCLUDE_TASK_THREAD_H

// LINKED
#include "utils.h"

// POSIX
#include <pthread.h>
#include <semaphore.h>

// INCLUDE
#include <parser.h>

//==============================================================================
// EXTERNAL

/*  Enums --------------------------------------------------------------------*/

typedef enum task_thread_state {WAIT=0, RUN, IDLE} task_thread_state_t;

/* Typedefs ------------------------------------------------------------------*/

/**
 * @brief holds the necessary attributes to define a threadable task object via
 *        the task_thread module.
 */
typedef struct task_thread_t {
    parser_task_t task;
    pthread_t task_thread;
    task_thread_state_t state;
    int64_t num_iters;
} task_thread_t;

typedef struct task_stack_t {
    task_thread_t task_thread_buffer[NTASKS];
    uint8_t top;
} task_stack_t;

//==============================================================================
// PUBLIC

/**
 * @brief 
 */
PUBLIC int32_t push_task_thread
(
    task_thread_t *new_task_thread,
    task_stack_t *task_stack
);

/**
 * @brief 
 */
PUBLIC int32_t pop_task_thread
(
    task_thread_t *return_task,
    task_stack_t *task_stack
);

/**
 * @brief Push a stack to be completed
 */
PUBLIC int32_t initialize_task_thread
(
    task_thread_t *new_task_thread,
    task_stack_t *held_tasks
);

/**
 * @brief Create a task thread and dispatch
 */
PUBLIC int32_t task_thread_create
(
    task_thread_t *waiting_task,
    void *(*routine)(void*)
);

/**
 * @brief Join a task with the calling thread
 */
PUBLIC int32_t task_thread_join(task_thread_t *running_task);

#endif // INCLUDE_TASK_THREAD_H
