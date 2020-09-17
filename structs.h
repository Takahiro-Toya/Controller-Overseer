#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

typedef struct
{
    int success, seconds, fileargscount, mem, memkill;
    char *logfile;
    char *outfile;
    char *execCommand;
    char *filename;
    char **fileargs;
} options_t;