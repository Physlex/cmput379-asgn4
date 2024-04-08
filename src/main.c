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

    return 0;
}
