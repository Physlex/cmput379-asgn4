// LINKED
#include "simulator.h"
#include "utils.h"

// UNIX
#include <unistd.h>
#include <errno.h>

// LINUX
#include <sys/stat.h>

// INCLUDE
#include "parser.h"

//==============================================================================
// EXTERNAL

//==============================================================================
// INTERNAL

// Simulator conifiguration details
PRIVATE simulator_config_t simulator_config;

// Resources read from the input file
PRIVATE volatile parser_resource_t simulator_resources[NRES_TYPES];

// Tasks read from the input file
PRIVATE volatile parser_task_t simulator_tasks[NTASKS];

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

PRIVATE uint32_t open_file(const char *input_filepath)
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

PRIVATE int32_t read_file_content(char buffer[TSK_LEN()])
{
    char line_buffer[TSK_LEN()];
    const int rsc_len = strlen("resources");
    const int tsk_len = strlen("task");

    // While we can continue reading lines with no error...
    for ( int i = 0; (read_line(buffer, simulator_input_fptr) == 0); ++i )
    {
        memset(line_buffer, 0, TSK_LEN());
        clean_line(buffer, line_buffer, TSK_LEN());

        if ( strncmp("resources", line_buffer, rsc_len) == 0 )
        {
            simulator_errorcode = parse_resources(
                line_buffer, &simulator_resources[i]
            );
        }
        else if ( strncmp("task", line_buffer, tsk_len) == 0 )
        {
            simulator_errorcode = parse_tasks(
                line_buffer, &simulator_tasks[i]
            );
        }
        else
        {
            char error_msg[STD_MSG_LEN];
            memset(&error_msg[0], 0, STD_MSG_LEN);

            sprintf(&error_msg[0], "No such command %s", line_buffer);
            error(&error_msg[0]);

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

PUBLIC uint32_t init_simulator(simulator_config_t *config)
{
    // Set up configuration for simulator

    simulator_config.monitor_time = config->monitor_time;
    simulator_config.num_iters = config->num_iters;
    strncpy(
        &simulator_config.input_filepath[0], &config->input_filepath[0],
        strlen(&config->input_filepath[0])
    );


    // Simulator file processing

    const char *input_filepath = &simulator_config.input_filepath[0];

    if ( open_file(input_filepath) > 0)
    {
        error("Simulator failed to open file");
        fclose(simulator_input_fptr);
        return simulator_errorcode;
    }

    char *file_buff[TSK_LEN()];
    memset(file_buff, 0, TSK_LEN());

    if ( read_file_content(file_buff) != SIMULATOR_OKAY_NERROR )
    {
        error("Simulator failed to read file content");
        fclose(simulator_input_fptr);
        return simulator_errorcode;
    }

    fclose(simulator_input_fptr);

    // End simulator file processing

    return simulator_errorcode;
}

PUBLIC uint32_t invoke_simulator()
{

// TODO: Write simulator invokation code here

    return simulator_errorcode;
}
