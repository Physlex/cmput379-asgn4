#ifndef INCLUDE_MAIN_H
#define INCLUDE_MAIN_H

// LINKED
#include "utils.h"

//==============================================================================
// PRIVATE

//==============================================================================
// PUBLIC

// Reads lines from a specified fileptr until returns a line that has neither a
// comment, nor an empty.
int read_line(FILE *fptr, char *line_buff, const int line_len) {

    memset(line_buff, 0, line_len);

    while (
        (strlen(line_buff) == 0) ||
        (strlen(line_buff) > 0 && line_buff[0] == '#')) {

        memset(line_buff, 0, line_len);
        if ( fgets(line_buff, line_len, fptr) == NULL ) {
            return 1;
        }
    }

    return 0;
}

PUBLIC int parse_resources() {


}

PUBLIC int parse_tasks() {

}

#endif // INCLUDE_MAIN_H_
