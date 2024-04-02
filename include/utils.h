#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H

// C includes that every file should have automatically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Forces a function to have internal linkage
#define PRIVATE static

// Forces a function to have external linkage (default)
#define PUBLIC ;

// Maximum number of bytes in a command
#define COMMAND_LEN 100000

// Arbitrary standard message length for unspecified funcitonality
#define STD_MSG_LEN 256

// Standardized length of each command token
#define TOKEN_LEN 32

// Generic Error, unspecified error type
#define GEN_ERROR 0x01

// Maximum number of resource types in a command
#define NRES_TYPES 10

// Maximum number of tasks in a command
#define NTASKS 25

#endif // INCLUDE_UTILS_H_
