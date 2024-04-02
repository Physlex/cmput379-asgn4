// LINKED
#include "main_test.h"
#include "utils.h"

int main (int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s log_folder\n", argv[0]);
        return GEN_ERROR;
    }

    const char *filepath = argv[1];
    if ( open_log_file(filepath) < 0 ) {
        return TEST_LOG_OPEN_ERROR;
    }

    printf("Running tests...\n\n");

    if ( check_test_list() < 0 ) {
        fprintf(stderr, "One or more tests failed\n");
        return TEST_FAILED_ERROR;
    }

    close_log_file();

    return 0;
}
