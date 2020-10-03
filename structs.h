#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

/*
 * option type Enum
*/
typedef enum op_type
{
    Mem = 0,
    MemWithPid = 1,
    Memkill = 2,
    FileExec = 3
} op_type_t;

/*
 * option struct used in controller
*/
typedef struct
{
    op_type_t type;
    int success, seconds, mempid, memkill, execArgc;
    char *logfile;
    char *outfile;
    char *execCommand;
} options_t;

/*
 * option struct used in overseer
*/
typedef struct
{
    bool useOut, useLog;
    op_type_t type;
    int request_id, seconds, mempid, memkill, execArgc;
    char *logfile;
    char *outfile;
    char *execCommand;
} options_server_t;

/*
 * container for linked list that holds options_server_t 
 * that is waiting for execution
 */
typedef struct optionContainer optionContainer_t;

struct optionContainer
{
    int log_fd, out_fd;
    options_server_t *option;
    optionContainer_t *next;
};

/*
 * container for storing memory report as linked list
 * */
typedef struct mem_entry mem_entry_t;
struct mem_entry
{
    int id;
    pid_t pid;
    int bytes;
    char *time;
    mem_entry_t *next;   
};

/*
 * all requests made in controller is saved in this struct and linked list
 * */
typedef struct saved_request saved_request_t;
struct saved_request
{
    int request_id;
    char *file_args;
    saved_request_t *next;
};

#endif