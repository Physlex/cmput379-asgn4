// LINKED
#include "simulator.h"
#include "utils.h"

// UNIX
#include <unistd.h>
#include <errno.h>

// INCLUDE
#include "parser.h"
#include "task_monitor.h"

//==============================================================================
// INTERNAL

/* Global Variables ----------------------------------------------------------*/

// Simulator conifiguration details
PRIVATE simulator_config_t simulator_config;

// Resources read from the input file
PRIVATE parser_resource_t simulator_resources[NRES_TYPES];

// Tasks read from the input file
PRIVATE parser_task_t simulator_tasks[NTASKS];

// Input file for the simulator
PRIVATE FILE *simulator_input_fptr = NULL;

// Last known error within a sim-step, does not account for running the sim
PRIVATE volatile int simulator_errorcode = 0;

//==============================================================================
// EXTERNAL


//==============================================================================
// PRIVATE

PRIVATE void error(const char *error_msg)
{
    fprintf(stderr, "Simulator Error [%d]: %s\n", simulator_errorcode, error_msg);
    return;
}

PRIVATE int32_t open_file(const char *input_filepath)
{
    if ( (simulator_input_fptr = fopen(input_filepath, "r")) == NULL )
    {
        char error_msg[STD_MSG_LEN];
        memset(&error_msg[0], 0, STD_MSG_LEN);

        sprintf(&error_msg[0], "File %s not found\n", input_filepath);
        fprintf(stderr, "%s: %s\n", strerror(errno), &error_msg[0]);

        simulator_errorcode = SIMERR_FNFND;
    } else
    {
        printf("Simulator: [File Open] %s\n", input_filepath);
    }

    return simulator_errorcode;
}

PRIVATE int32_t load_simulator_config_file()
{
    char dirty_buffer[COMMAND_LEN];
    char clean_buffer[TSK_LEN()];
    const int rsc_len = strlen("resources");
    const int tsk_len = strlen("task");

    // While we can continue reading lines with no error...
    for ( int i = 0; (read_line(dirty_buffer, simulator_input_fptr) == 0); ++i )
    {
        memset(clean_buffer, 0, TSK_LEN());
        clean_line(dirty_buffer, clean_buffer);

        if ( strncmp("resources", clean_buffer, rsc_len) == 0 )
        {
            simulator_errorcode = parse_resources(
                clean_buffer, &simulator_resources[i]
            );
        }
        else if ( strncmp("task", clean_buffer, tsk_len) == 0 )
        {
            simulator_errorcode = parse_tasks(
                clean_buffer, &simulator_tasks[i]
            );
        }
        else
        {
            error("Failed to parse command");

            simulator_errorcode = SIMULATOR_PARSE_ERROR;
        }
    }

    if (simulator_errorcode != SIMULATOR_OKAY_NERROR)
    {
        char error_msg[STD_MSG_LEN];
        memset(&error_msg[0], 0, STD_MSG_LEN);
        sprintf(
            &error_msg[0],
            "Failed with code %d",
            simulator_errorcode
        );

        error(&error_msg[0]);
    }

    return simulator_errorcode;
}

//==============================================================================
// PUBLIC

PUBLIC int32_t init_simulator(simulator_config_t *config)
{
    memcpy(&simulator_config, config, sizeof(*config));
    const char *input_filepath = &simulator_config.input_filepath[0];

    if ( open_file(input_filepath) != SIMULATOR_OKAY_NERROR)
    {
        error("Simulator failed to open file");
        fclose(simulator_input_fptr);
        return simulator_errorcode;
    }

    if ( load_simulator_config_file() != SIMULATOR_OKAY_NERROR )
    {
        error("Simulator failed to read file content");
        fclose(simulator_input_fptr);
        return simulator_errorcode;
    }

    fclose(simulator_input_fptr);

    return simulator_errorcode;
}

PUBLIC int32_t invoke_simulator()
{
    if ( lock_resources(&simulator_resources[0]) < 0 )
    {
        error("Failed to lock resources");
        return -1;
    }

    const size_t niters = simulator_config.num_iters;
    for (int i = 0; i < NTASKS; ++i)
    {
        parser_task_t *curr_task = &simulator_tasks[i];

        if (strlen(curr_task->name) == 0)
        {
            break; // From this point on there are no tasks defined
        }

        if ( dispatch_task_thread(&simulator_tasks[i], niters) < 0 )
        {
            error("Failed to dispatch thread");
            return -1;
        }
    }

    if ( wall_tasks() < 0 )
    {
        error("Failed to wall threads");
        return -1;
    }

    return simulator_errorcode;
}
