// LINKED
#include "utils.h"
#include "parser.h"

// UNIX
#include <errno.h>
#include <unistd.h>

// LINUX
#include <sys/types.h>
#include <sys/stat.h>

//==============================================================================
// PRIVATE

PRIVATE char to_lower(char old)
{
    char *new = &old;
    const int offset = 'a' - 'A';

    if (old >= 'A' && old <= 'Z')
    {
        *new = offset + old;
    }

    return *new;
}

PRIVATE int32_t scan(const char *src, parser_resource_t *rsc_ptr)
{
    const size_t resource_len = TOKEN_LEN << 1;

    for (int i = 0; i < NRES_TYPES; ++i)
    {
        // Read name
 
        for (int j = 0; j < TOKEN_LEN; ++j)
        {
            rsc_ptr[i].name[j] = src[(i * resource_len) + j];
        }


        // Read value

        for (int k = 0; k < TOKEN_LEN; ++k)
        {
            rsc_ptr[i].value[k] = src[(i * resource_len) + (TOKEN_LEN + k)];
        }
    }


    return 0;
}

//==============================================================================
// PUBLIC

PUBLIC int32_t read_line(char buffer[COMMAND_LEN], FILE *input_file)
{
    char *err = fgets(&buffer[0], COMMAND_LEN, input_file);
    if (err == NULL) { return 1; }

    while ( (strlen(&buffer[0]) != 0) &&
          ( (buffer[0] == ' ') || (buffer[0] == '#') || (buffer[0] == '\n') ))
    {
        memset(&buffer[0], 0, COMMAND_LEN);
        err = fgets(&buffer[0], COMMAND_LEN, input_file);
        if (err == NULL) { return 1; }
    }

    buffer[strlen(&buffer[0]) - 1] = '\0';

    return 0;
}

PUBLIC int32_t clean_line(const char *src, char *dst)
{
    bool is_first_ws = true;

    for (size_t i = 0, j = 0, k = 0; i < COMMAND_LEN; ++i)
    {
        char src_char = src[i];
        const char curr_char = to_lower(src_char);

        if (curr_char == ' ' || curr_char == '\t')
        {
            if (is_first_ws == true)
            {
                while (k < TOKEN_LEN)
                {
                    dst[j] = '\0';
                    ++j;
                    ++k;
                }

                k = 0;
            }

            is_first_ws = false;
        }
        else if (curr_char == ':')
        {
            while (k < TOKEN_LEN)
            {
                dst[j] = '\0';
                ++j;
                ++k;
            }

            k = 0;
        }
        else
        {
            dst[j] = curr_char;
            ++j;
            ++k;

            is_first_ws = true;
        }
    }

    return 0;    
}

PUBLIC int32_t parse_tasks(const char *src, parser_task_t *tsk_ptr)
{
    // Offset task

    const char *tmp = src + TOKEN_LEN;

    // Copy task name

    strcpy(tsk_ptr->name, tmp);
    tmp += TOKEN_LEN;

    // Copy busyTime of task

    char busy_time_str[TOKEN_LEN];
    memset(&busy_time_str[0], 0, TOKEN_LEN);
    strcpy(&busy_time_str[0], tmp);

    tsk_ptr->busy_time = atoi(&busy_time_str[0]);
    if (tsk_ptr->busy_time < 0)
    {
        fprintf(
            stderr,
            "Parser Error: busy time %s not valid integer\n",
            &busy_time_str[0]
        );

        return -1;
    }

    tmp += TOKEN_LEN;

    // Copy idleTime of task

    char idle_time_str[TOKEN_LEN];
    memset(&idle_time_str[0], 0, TOKEN_LEN);
    strcpy(&idle_time_str[0], tmp);

    tsk_ptr->idle_time = atoi(&idle_time_str[0]);
    if (tsk_ptr->idle_time < 0)
    {
        fprintf(
            stderr,
            "Parser Error: idle time %s not valid integer\n",
            &idle_time_str[0]
        );

        return -1;
    }

    tmp += TOKEN_LEN;

    // Copy resources of task

    if ( scan(tmp, &tsk_ptr->resources[0]) < 0 )
    {
        fprintf(stderr, "Parser Error: task resource scan failed\n");
        return -1;
    }

    return 0;
}

PUBLIC int32_t parse_resources(const char *src, parser_resource_t *rsc_ptr)
{
    if ( scan(src + TOKEN_LEN, rsc_ptr) < 0 )
    {
        fprintf(stderr, "Parser Error: parse resource scan failed\n");
        return -1;
    }
    else
    {
        return 0;
    }
}
