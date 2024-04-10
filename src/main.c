// LINKED
#include "utils.h"

// INCLUDE
#include "simulator.h"

int main (int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s inputFile monitorTime NITER\n", argv[0]);
        exit(GEN_ERROR);
    }

    simulator_config_t config;
    strncpy(&config.input_filepath[0], argv[1], strlen(argv[1]));
    config.monitor_time = atoi(argv[2]);
    if (config.monitor_time < 0)
    {
        fprintf(stderr, "Error: monitorTime must be positive integral value\n");
        fprintf(stderr, "Usage: %s inputFile monitorTime NITER\n", argv[0]);
        exit(GEN_ERROR);
    }

    config.num_iters = atoi(argv[3]);
    if (config.monitor_time < 0)
    {
        fprintf(stderr, "Error: numIters must be positive integral value\n");
        fprintf(stderr, "Usage: %s inputFile monitorTime NITER\n", argv[0]);
        exit(GEN_ERROR);
    }

    if ( init_simulator(&config) != SIMULATOR_OKAY_NERROR )
    {
        fprintf(stderr, "Failed to initialize simulator\n");
        exit(GEN_ERROR);
    }

    if ( invoke_simulator() != SIMULATOR_OKAY_NERROR )
    {
        fprintf(stderr, "Failed to invoke simulator\n");
        exit(GEN_ERROR);
    }

    return 0;
}
