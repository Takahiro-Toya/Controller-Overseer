#ifndef _STRUCTS_H
#define _STRUCTS_H

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

typedef struct
{
    bool useOut, useLog;
    int seconds, mem, memkill, execArgc;
    char *logfile;
    char *outfile;
    char *execCommand;
} options_server_t;

struct optionContainer
{
    int log_fd, out_fd;
    options_server_t *option;
    optionContainer_t *next;
};

#endif