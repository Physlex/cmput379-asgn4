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

PRIVATE uint32_t read_file_content(char *file_buff, const size_t file_len)
{
    return 0;
}

//==============================================================================
// PUBLIC

PUBLIC uint32_t init_simulator(simulator_config_t *config)
{
    // Set up configuration for simulator

    ASSERT_TRUE(config);

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

    struct stat st_buff;
    stat(input_filepath, &st_buff);
    const size_t file_len = st_buff.st_size;

    char *file_buff = (char*)malloc(file_len);
    memset(file_buff, 0, file_len);
    ASSERT_TRUE(file_buff);

    if ( read_file_content(file_buff, file_len) > 0 )
    {
        error("Simulator failed io read file content");
        free(file_buff);
        fclose(simulator_input_fptr);
        return simulator_errorcode;
    }

    free(file_buff);
    fclose(simulator_input_fptr);

    // End simulator file processing


    return simulator_errorcode;
}

PUBLIC uint32_t invoke_simulator()
{

// TODO: Write simulator invokation code here

    return simulator_errorcode;
}
