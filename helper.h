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



/*
 * printf int, to debug
 */
void print_int(int i)
{
    printf("%d\n", i);
}
/*
 * printf string, to debug
 */
void print_string(char *string)
{
    printf("%s\n", string);
}




#endif