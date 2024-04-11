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

// Conversion from value=str to value=int
typedef struct task_monitor_resource_t {
    char name[TOKEN_LEN];
    int amnt;
} task_monitor_resource_t;

/**
 * @brief A resource that holds a lock, to determine it's availablity
 */
typedef struct task_monitor_resource_lock_t {
    task_monitor_resource_t resource;
    sem_t available_resource;
} task_monitor_resource_lock_t;

//==============================================================================
// PUBLIC

// Lock system resources for use in tasks
PUBLIC int32_t lock_resources(parser_resource_t *resources);

PUBLIC int32_t push_task
(
    parser_task_t *new_task,
    const uint64_t num_iters
);

/**
 * @brief Creates a monitor task thread to output task state every delay_ms
 */
PUBLIC int32_t dispatch_monitor_thread(const size_t delay_ms);

/**
 * @brief Creates a task thread from loaded thread through @ref push_task
 */
PUBLIC int32_t dispatch_task_thread(void);

/**
 * @brief Task cleanup. Prints all system information as specified in the SRS.
 */
PUBLIC int32_t display_task_information(parser_resource_t *resources_config);

/**
 * @brief Join all dispatched tasks to the main thread
 */
PUBLIC int32_t wall_tasks();

#endif // INCLUDE_TASK_MONITOR_H
