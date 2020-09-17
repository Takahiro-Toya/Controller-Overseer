#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

void timestamp() {
    time_t t = time(NULL);
    struct tm *local = localtime(&t);
    printf("%d-%d-%d %d:%d:%d ", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
}


char **split_string_by_space(char *string, int splitnum)
{

    char copy[strlen(string)];
    strcpy(copy, string);
    // char **strarray;
    char **strarray = (char **)malloc(sizeof(char *) * splitnum);
    char *p = strtok(copy, " ");

    for (int i = 0; i < splitnum; i++)
    {
        strarray[i] = (char *)malloc(sizeof(char) * strlen(p));
        strarray[i] = p;
        p = strtok(NULL, " ");
    }

    free(p);

    return strarray;
}

char **get_args_from_arg_array(char **argarray, int count)
{
    char **array = (char **)malloc(sizeof(char *) * count);
    for (int i = 1; i < count; i++)
    {
        array[i] = (char *)malloc(sizeof(char) * strlen(argarray[i]));
        array[i] = argarray[i];
    }

    return array;
}