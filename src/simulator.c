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
PRIVATE volatile int simulator_errorcode = SIMULATOR_OKAY_NERROR;

//==============================================================================
// EXTERNAL


//==============================================================================
// PRIVATE

PRIVATE void error(const char *error_msg)
{
    fprintf(stderr, "Simulator Error [0x%x]: %s\n", simulator_errorcode, error_msg);
    return;
}

PRIVATE int32_t open_file(const char *input_filepath)
{
    if ( (simulator_input_fptr = fopen(input_filepath, "r")) == NULL )
    {
        char error_msg[STD_MSG_LEN];
        memset(&error_msg[0], 0, STD_MSG_LEN);

        sprintf(&error_msg[0], "File %s not found", input_filepath);
        fprintf(stderr, "%s: %s\n", strerror(errno), &error_msg[0]);

        simulator_errorcode = SIMERR_FNFND;
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

    uint8_t parse_error = 0;
    int i = 0;
    while (read_line(dirty_buffer, simulator_input_fptr) == 0)
    {
        memset(clean_buffer, 0, TSK_LEN());
        clean_line(dirty_buffer, clean_buffer);

        if ( strncmp("resources", clean_buffer, rsc_len) == 0 )
        {
            parse_error = parse_resources(
                clean_buffer, &simulator_resources[0]
            );
        }
        else if ( strncmp("task", clean_buffer, tsk_len) == 0 )
        {
            parse_error = parse_tasks(
                clean_buffer, &simulator_tasks[i]
            );
            ++i;
        }
        else
        {
            error("Failed to parse command");
            simulator_errorcode = SIMULATOR_PARSE_ERROR;
        }
    }

    if (parse_error != 0) { simulator_errorcode = SIMULATOR_PARSE_ERROR; }

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
        simulator_errorcode = SIMERR_FNFND;
        error("Simulator failed to open file");
        return simulator_errorcode;
    }
    else
    {
        printf("Simulator opened file %s\n", input_filepath);
    }

    if ( load_simulator_config_file() != SIMULATOR_OKAY_NERROR )
    {
        error("Simulator failed to read file content");
        fclose(simulator_input_fptr);
        return simulator_errorcode;
    }
    else
    {
        printf("Simulator correctly parsed file content\n");
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
    else
    {
        printf("Simulator resources locked\n");
    }

    const size_t num_iters = simulator_config.num_iters;
    for (int i = 0; i < NTASKS; ++i)
    {
        if (strlen(simulator_tasks[i].name) == 0)
        {
            break; // From this point on there are no tasks defined
        }

        if ( push_task(&simulator_tasks[i], num_iters) < 0 )
        {
            error("Task failed to be added to stack\n");
            return -1;
        }

        if ( dispatch_task_thread() < 0 )
        {
            char error_msg[STD_MSG_LEN + TOKEN_LEN];
            memset(&error_msg[0], 0, sizeof(error_msg));

            sprintf
            (
                &error_msg[0],
                "Failed to dispatch task %s\n",
                &simulator_tasks[i].name[0]
            );
            error(&error_msg[0]);

            return -1;
        }
    }

    if ( dispatch_monitor_thread(simulator_config.monitor_time) < 0 )
    {
        error("Failed to dispatch monitor thread");
        return -1;
    }
    else
    {
        printf("Monitor thread dispatched\n\n");
    }

    if ( wall_tasks() < 0 )
    {
        error("Failed to wall threads");
        return -1;
    }
    else
    {
        printf("\nJoined all threads to main\n\n");
    }

    printf("\n");

    return simulator_errorcode;
}
