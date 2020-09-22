#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

typedef struct optionContainer optionContainer_t;

typedef struct
{
    int success, seconds, mem, memkill, execArgc;
    char *logfile;
    char *outfile;
    char *execCommand;
} options_t;

struct optionContainer
{
    options_t *option;
    optionContainer_t *next;
};
