#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include "output_manage.h"


int stdout_copy = -1;
int stderr_copy = -1;

// int current = 0; // 0: std, 1: out, 2: log

/**
 * copy default stdout/stderr file descriptor 
 * so that you can restore back to std later
 * Call this method once and common to all processes/threads
 */
void use_fd() 
{
    if (stdout_copy < 0 && stderr_copy < 0) {
        stdout_copy = dup(1);
        stderr_copy = dup(2);
    }
}

void force_reset()
{
    if (stdout_copy > 0 && stderr_copy > 0) {
        dup2(stdout_copy, 1);
        dup2(stderr_copy, 2);
    }
}

/*
 * open the file and set file descriptor number to des
 * you should use the new value set to des to change redirection
 */
void set_fd(char *file, int *des) 
{
    if (file == NULL)
    {
        *des = -1;
    } 
    else 
    {
        if ((*des = open(file, O_CREAT | O_APPEND | O_WRONLY, 0666)) < 0)
        {
            perror("open file");
            exit(1);
        }
    }
}

int get_stdout_copy_fd()
{
    return stdout_copy;
}

int get_stderr_copy_fd()
{
    return stderr_copy;
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