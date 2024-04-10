#ifndef INCLUDE_SIMULATOR_H
#define INCLUDE_SIMULATOR_H

// LINKED
#include "utils.h"

// DEFINITIONS

#define SIMERR_BASE 0x1000UL
#define SIMERR_FNFND SIMERR_BASE + 0x01UL

//==============================================================================
// EXTERNAL

// Holds configuration types for the purposes of initializing the simulator
typedef struct simulator_config_t {
    char input_filepath[FILENAME_MAX];
    int monitor_time;
    int num_iters;
} simulator_config_t;

//==============================================================================
// PUBLIC

// Starts the simulator and loads config file
PUBLIC int32_t init_simulator(simulator_config_t *config);

// Invokes the simulator to run tasks
PUBLIC int32_t invoke_simulator();

#endif // INCLUDE_SIMULATOR_H
