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



