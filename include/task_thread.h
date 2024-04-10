#ifndef INCLUDE_TASK_THREAD_H
#define INCLUDE_TASK_THREAD_H

// LINKED
#include "utils.h"

// POSIX
#include <pthread.h>
#include <semaphore.h>

//==============================================================================
// EXTERNAL

/*  Enums --------------------------------------------------------------------*/

typedef enum task_thread_state {WAIT=0, RUN, IDLE} task_thread_state_t;

/* Forward Declarations ------------------------------------------------------*/

typedef struct parser_task_t parser_task_t;

/* Typedefs ------------------------------------------------------------------*/

/**
 * @brief holds the necessary attributes to define a threadable task object via
 *        the task_thread module.
 */
typedef struct task_thread_t {
    parser_task_t *task;
    pthread_t task_thread;
    task_thread_state_t state;
} task_thread_t;

/**
 * @brief A resource that holds a lock, to determine it's availablity
 */
typedef struct task_thread_resource_lock_t {
    parser_resource_t *resource;
    sem_t available_resource;
} task_thread_resource_lock_t;

typedef struct task_stack_t {
    task_thread_t task_thread_buffer[NTASKS];
    uint8_t top;
} task_stack_t;

//==============================================================================
// PUBLIC

/**
 * @brief Locks the resources from the simulator into the task_thread manager
 */
PUBLIC int32_t lock_resources();

/**
 * @brief Queue a task thread object to be run
 */
PUBLIC int32_t initialize_task_thread(task_thread_t *new_task_thread);

/**
 * @brief Takes the next task within the task queue and executes the
 *        task_monitor routine
 */
PUBLIC int32_t dispatch_task_thread();

/**
 * @brief Waits for all tasks to complete their iterations
 */
PUBLIC int32_t wall_tasks()

#endif // INCLUDE_TASK_THREAD_H
