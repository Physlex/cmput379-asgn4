#ifndef INCLUDE_PARSER_H
#define INCLUDE_PARSER_H

// LINKED
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
// PUBLIC

// Creates a dirty line that must be cleaned. Skips comments and empty lines
PUBLIC int32_t read_line(char buffer[COMMAND_LEN], FILE *input_file)

// Removes white space and other non-token characters, packages into dst
PUBLIC int32_t clean_line(const char *src, char *dst)

// Parses resources from src into formatted rsc_ptr
PUBLIC int32_t parse_resources(const char *src, parser_resource_t *rsc_ptr);

// Parses tasks from src into formatted tsk_ptr
PUBLIC int32_t parse_tasks(const char *src, parser_task_t *tsk_ptr);

#endif // INCLUDE_PARSER_H
