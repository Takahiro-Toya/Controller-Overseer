
#include "helper.h"

/*
 * printf int, to debug
 */
void print_int(int i)
{
    printf("%d\n", i);
}
/*
 * printf string, to debug
 */
void print_string(char *string)
{
    printf("%s\n", string);
}

/*
test
 * Chceck if the string is number 
 */
int is_number(const char *str) {

    int length = strlen(str);
    for (int i=0; i < length; i++)
        if (!isdigit(str[i]))
        {
            return 0;
        }
    return 1;
}

/*
 * Split string by every space and store them into an array of string
 * Specify the number of split (splitnum) so it won't run 'for loop' twice inside
 * returns the result array
 */
char **split_string_by_space(char *string, int splitnum)
{
    char copy[strlen(string) + 1];
    strcpy(copy, string);

    char **strarray = (char **)exMalloc(sizeof(char *) * (splitnum + 1));
    char *p = strtok(copy, " ");

    for (int i = 0; i < splitnum; i++)
    {
        strarray[i] = (char *)exMalloc((sizeof(char) + 1) * strlen(p));
        strcpy(strarray[i], p);
        p = strtok(NULL, " ");
    }

    free(p);

    strarray[splitnum] = NULL;

    return strarray;
}