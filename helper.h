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
 * printf timestamp Y-M-d h-m-s
 *
 */
void timestamp()
{
    time_t t = time(NULL);
    struct tm *local = localtime(&t);
    printf("%d-%d-%d %d:%d:%d ", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
}

/*
 * Split string by every space and store them into an array of string
 * Specify the number of split (splitnum) so it won't run 'for loop' twice inside
 * returns the result array
 */
char **split_string_by_space(char *string, int splitnum)
{

    char copy[strlen(string)];
    strcpy(copy, string);

    char **strarray = (char **)malloc(sizeof(char *) * (splitnum + 1));
    char *p = strtok(copy, " ");

    for (int i = 0; i < splitnum; i++)
    {
        strarray[i] = (char *)malloc(sizeof(char) * strlen(p));
        strcpy(strarray[i], p);
        p = strtok(NULL, " ");
    }

    free(p);

    strarray[splitnum] = NULL;

    return strarray;
}

/*
 * printf int
 */
void print_int(int i)
{
    printf("%d\n", i);
}
/*
 * printf string
 */
void print_string(char *string)
{
    printf("%s\n", string);
}

void set_dest(char *file)
{
    if (file != NULL)
    {
        int efd;
        if ((efd = open(file, O_CREAT | O_WRONLY, 0666)) < 0)
        {
            exPerror("open logfile");
        }
        else
        {
            dup2(efd, 1);
            close(efd);
        }
    }
}

int open_dest(char *file)
{
    int efd = -1;
    if (file != NULL)
    {
        if ((efd = open(file, O_CREAT | O_WRONLY, 0666)) < 0)
        {
            exPerror("open logfile");
        }
        else
        {
            dup2(efd, 1);
            return efd;
        }
    }

    return efd;
}

void close_dest(int fd)
{
    if (!(fd < 0))
    {
        close(fd);
    }
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
