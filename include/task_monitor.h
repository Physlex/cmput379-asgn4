#ifndef INCLUDE_TASK_MONITOR_H
#define INCLUDE_TASK_MONITOR_H

#include "utils.h"

//==============================================================================
// EXTERNAL

/* Forward Declarations ------------------------------------------------------*/

typedef struct parser_task_t parser_task_t;

/* Typedefs ------------------------------------------------------------------*/

typedef struct task_monitor_config_t {
    uint32_t idle_time;
    uint32_t busy_time;
} task_monitor_config_t;

//==============================================================================
// PUBLIC

PUBLIC int32_t acquire();

PUBLIC int32_t release();

#endif // INCLUDE_TASK_MONITOR_H
