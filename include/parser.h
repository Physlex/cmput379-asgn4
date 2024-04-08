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

// Defines a line of tasks read by the simulator from the input file
typedef struct {
    char task_name[STD_MSG_LEN];
    int busytime;
    int idleTime;
    parser_token_pair_t task_table[NTASKS]; // name1:value1, name2:value2,...
} parser_task_t;

// Redefinition of resource type for consistency
typedef parser_token_pair_t parser_resource_t;

//==============================================================================
// PRIVATE

//==============================================================================
// PUBLIC

PUBLIC int32_t read_line();

#endif // INCLUDE_PARSER_H
