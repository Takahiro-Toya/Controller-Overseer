#ifndef _HELPER_H
#define _HELPER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <ctype.h>
#include "extensions.h"

/*
 * printf int, to debug
 */
void print_int(int i);
/*
 * printf string, to debug
 */
void print_string(char *string);

/*
 * Chceck if the string is number 
 */
int is_number(const char *str);

/*
 * Split a argment string by a space into splitnum so that you can split into
 * file name and its arguments 
 */
char **split_string_by_space(char *string, int splitnum);


#endif