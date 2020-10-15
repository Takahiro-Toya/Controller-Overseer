
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