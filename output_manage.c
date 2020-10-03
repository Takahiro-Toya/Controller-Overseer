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


/**
 * copy default stdout/stderr file descriptor 
 * so that you can restore back to std later
 * Call this method once and common to all processes/threads
 */
void use_fd()
{
    if (stdout_copy < 0 && stderr_copy < 0)
    {
        stdout_copy = dup(1);
        stderr_copy = dup(2);
    }
}

void force_reset()
{
    if (stdout_copy > 0 && stderr_copy > 0)
    {
        dup2(stdout_copy, 1);
        dup2(stderr_copy, 2);
    }
}

void close_std_copy() {
    close(stdout_copy);
    close(stderr_copy);
}

/*
 * Get a file descriptor for the given file path.
 * If the file does not exist in the path, then create a new file
 * returns -1 if the given parameter is NULL
 * Exit the process if the open file failed
 */
int get_fd(char *file)
{
    int des;
    if (file == NULL)
    {
        des = -1;
    }
    else
    {
        if ((des = open(file, O_CREAT | O_APPEND | O_WRONLY, 0666)) < 0)
        {
            perror("open/create file");
            exit(1);
        }
    }
    return des;
}



/*
 * printf timestamp Y-M-d h-m-s
 */
void timestamp()
{
    time_t t = time(NULL);
    struct tm *locale = localtime(&t);
    printf("%d-%d-%d %d:%d:%d ", locale->tm_year + 1900, locale->tm_mon + 1, locale->tm_mday, locale->tm_hour, locale->tm_min, locale->tm_sec);
}

char *get_formatted_time()
{
    char *time_buf;
    time_buf = malloc(sizeof(char) * 20);
    time_t t = time(NULL);
    struct tm *locale = localtime(&t);
    sprintf(time_buf, "%d-%d-%d %d:%d:%d", locale->tm_year + 1900, locale->tm_mon + 1, locale->tm_mday, locale->tm_hour, locale->tm_min, locale->tm_sec);
    return time_buf;
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