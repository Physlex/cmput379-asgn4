#ifndef INCLUDE_TASK_MONITOR_H
#define INCLUDE_TASK_MONITOR_H

// LINKED
#include "utils.h"

// POSIX
#include <semaphore.h>

// INCLUDE
#include "parser.h"

//==============================================================================
// EXTERNAL


/* Typedefs ------------------------------------------------------------------*/

typedef struct task_monitor_config_t {
    uint32_t idle_time;
    uint32_t busy_time;
} task_monitor_config_t;

/**
 * @brief A resource that holds a lock, to determine it's availablity
 */
typedef struct task_monitor_resource_lock_t {
    parser_resource_t *resource;
    sem_t available_resource;
} task_monitor_resource_lock_t;

//==============================================================================
// PUBLIC

// Lock system resources for use in tasks
PUBLIC int32_t lock_resources(parser_resource_t *resources);

// Dispatch a thread task
PUBLIC int32_t dispatch_task_thread
(
    parser_task_t *new_task,
    const uint64_t num_iters
);

/**
 * @brief Join all dispatched tasks to the main thread
 */
PUBLIC int32_t wall_tasks();

#endif // INCLUDE_TASK_MONITOR_H