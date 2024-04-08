#ifndef TEST_MAIN_TEST_H
#define TEST_MAIN_TEST_H

// C
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// UNIX
#include <errno.h>
#include <unistd.h>

// INTERNAL
#include "utils.h"
#include "parsing_test.h"

//==============================================================================
// EXTERNAL LINKAGE

#define NUM_TESTS NUM_PARSING_TESTS

typedef uint8_t (*test_functor_t)(void);

//==============================================================================
// INTERNAL LINKAGE

static uint8_t g_idx = 0;
static FILE *g_log_fptr = NULL;

//==============================================================================
// PRIVATE

PRIVATE uint8_t log_test_results(uint8_t error_code) {

    char log_message[STD_MSG_LEN];
    memset(&log_message, 0, STD_MSG_LEN);

    if (error_code > 0) {
        sprintf(
            &log_message[0],
            "Error: Test [%d] failed with error code: [%d]\n",
            g_idx, error_code
        );
    } else {
        sprintf(
            &log_message[0],
            "Test [%d] finished execution successfully with code: [%d]\n",
            g_idx, error_code
        );
    }

    if ( fputs(&log_message[0], g_log_fptr) < 0 ) {
        perror("Failed to put to log file stream\n");
        fclose(g_log_fptr);
        return -1;
    }

    printf("Logged result for test [%d]\n", g_idx);

    return 0;
}

//==============================================================================
// PUBLIC

// Creates a log fptr, calling an error if it fails to open
PUBLIC int open_log_file(const char *filepath) {

    if (filepath == NULL) {
        fprintf(stderr, "Error: filepath is NULL\n");
        exit(GEN_ERROR);
    }

    const char *basepath = "logs/";
    const char *extension = ".txt";

    const uint8_t root_len = strlen(basepath) + strlen(filepath) + strlen(extension);
    char *rootpath = (char *)malloc(root_len);
    memset(rootpath, 0, root_len);

    for (g_idx = 0; g_idx < strlen(basepath); ++g_idx) {
        rootpath[g_idx] = basepath[g_idx];
    }

    strncat(rootpath, filepath, strlen(filepath));
    strncat(rootpath, extension, strlen(extension));

    if ( (g_log_fptr = fopen(rootpath, "w")) == NULL ) {
        fprintf(
            stderr,
            "Error: file %s failed to open\n",
            rootpath
        );

        return -1;
    }

    free(rootpath);

    return 0;
}

PUBLIC void close_log_file(void) {
    fclose(g_log_fptr);
}

// 'checks' the test list, making sure each one passes and logs the result
PUBLIC int check_test_list(void) {

    test_functor_t test_list[NUM_TESTS];
    memset(test_list, 0, sizeof(test_functor_t) * NUM_TESTS);
    // test_list[0] = test_main;

    for (g_idx = 0; g_idx < NUM_TESTS; ++g_idx) {
        test_functor_t curr_test = test_list[g_idx];
        const uint8_t error_code = curr_test();
        if ( log_test_results(error_code) < 0 ) {
            fprintf(stderr, "Error: Log test result failed to execute\n");
            return -1;
        }
    }

    return 0;
}

#endif // TEST_MAIN_TEST_H_
