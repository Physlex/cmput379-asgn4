#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H

//==============================================================================
// STANDARD INCLUDES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

//==============================================================================
// UTILITY

#if TEST_LVL > 0
#define ASSERT_TRUE(condition) (assert(value))
#elif TEST_LVL == 0
#define ASSERT_TRUE(condition) ;
#endif

// Forces a function to have internal linkage
#define PRIVATE static

// Forces a function to have external linkage (default)
#define PUBLIC ;

//==============================================================================
// LENGTHS

// Maximum number of bytes in a command
#define COMMAND_LEN 100000

// Arbitrary standard message length for unspecified funcitonality
#define STD_MSG_LEN 256

// Standardized length of each command token
#define TOKEN_LEN 32

#define TSK_LEN() (TOKEN_LEN << 2) + ((TOKEN_LEN << 1) * NRES_TYPES)

//==============================================================================
// ERROR CODES

// Generic Error, unspecified error type
#define GEN_ERROR 0x01

// Parser reading error, of some kind
#define SIMULATOR_PARSE_ERROR 0x11
#define SIMULATOR_OKAY_NERROR 0x12

// Test based errors, specifiec by an ascending first 4 bits
#define TEST_LOG_OPEN_ERROR 0x10
#define TEST_FAILED_ERROR 0x20

//==============================================================================
// RANDOM ASSIGNMENT DEFINITIONS

// Maximum number of resource types in a command
#define NRES_TYPES 10

// Maximum number of tasks in a command
#define NTASKS 25

#endif // INCLUDE_UTILS_H_
