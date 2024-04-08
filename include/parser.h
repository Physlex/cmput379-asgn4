#ifndef INCLUDE_PARSER_H
#define INCLUDE_PARSER_H

#include "utils.h"

//==============================================================================
// EXTERNAL

// Definiton of a key, value pair defined by a token defined as name:value
typedef struct {
    char name[TOKEN_LEN];
    char value[TOKEN_LEN];
} parser_token_pair_t;

// Redefinition of resource type for consistency
typedef parser_token_pair_t parser_resource_t;

// Defines a line of tasks read by the simulator from the input file
typedef struct {
    char name[TOKEN_LEN];
    int busy_time;
    int idle_time;
    parser_resource_t resources[NRES_TYPES];
} parser_task_t;


//==============================================================================
// PRIVATE

//==============================================================================
// PUBLIC

PUBLIC int32_t read_line();

#endif // INCLUDE_PARSER_H
