#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

typedef struct
{
    int success, seconds, mem, memkill, execArgc;
    char *logfile;
    char *outfile;
    char *execCommand;
} options_t;

typedef struct
{
    options_t *option;
    optionContainer_t *next;
} optionContainer_t

