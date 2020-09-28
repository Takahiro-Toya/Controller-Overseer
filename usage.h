#ifndef _USAGE_H
#define _USAGE_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "structs.h"
#include "extensions.h"

/*
 * print usage message to stderr
*/
void print_usage_error();

/*
 * Check the input arguments and retrieves each of arguments such as IP address, file names etc.
 * Print usage error if user inputs undefined command and returns struct options_t that stores the user input
 * options_t defined in structs.h 
 */
options_t getControllerInitCommand(int argc, char *argv[]);

#endif