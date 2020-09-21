#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include "output_manage.h"

int stdout_copy = -1;
int stderr_copy = -1;
int log_fd = -1;
int out_fd = -1;

int current = 0; // 0: std, 1: out, 2: log

void fd_init(char *outfile, char *logfile)
{
    stdout_copy = dup(1);
    stderr_copy = dup(2);
    if (outfile == NULL)
    {
        return;
    }
    else
    {
        if ((out_fd = open(outfile, O_CREAT | O_APPEND | O_WRONLY, 0666)) < 0)
        {
            perror("open outfile");
            exit(1);
        } 
    }
    if (logfile == NULL)
    {
        return;
    }
    else
    {
        if ((log_fd = open(logfile, O_CREAT | O_APPEND | O_WRONLY, 0666)) < 0)
        {
            perror("open logfile");
            exit(1);
        } 
    }   
}


void set_to_out()
{
    if (out_fd > 0)
    {
        dup2(out_fd, 1);
        dup2(2, 1);
        close(out_fd);
    }
}

void set_to_log()
{
    if (log_fd > 0)
    {
        dup2(log_fd, 1);
        close(log_fd);
    }
}

void set_to_default() 
{
    if (stdout_copy > 0 && stderr_copy > 0)
    {
        dup2(stdout_copy, 1);
        dup2(stderr_copy, 2);
        close(stdout_copy);
        close(stderr_copy);
    }
}

/*
 * printf timestamp Y-M-d h-m-s
 */
void timestamp()
{
    time_t t = time(NULL);
    struct tm *local = localtime(&t);
    printf("%d-%d-%d %d:%d:%d ", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
}


/*
 * Normal print following timestamp
 */
void print_log(const char *format, ...)
{
    timestamp();
    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end(va);
}