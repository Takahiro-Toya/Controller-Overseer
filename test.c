#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "usage.h"
#include "extensions.h"
#include "helper.h"


int main () {

    char *words = "Hello how are you ?";
    char **result = split_string_by_space(words, 5);
    for (int i = 0; i < 5; i++) {
        printf("%s\n", result[i]);
    }

    return 0;

}

// int main ()
// {
//     char buf[] ="abc/qwe/ccd";
//     int i = 0;
//     char *p = strtok (buf, "/");
//     char *array[4];

//     while (p != NULL)
//     {
//         array[i++] = p;
//         p = strtok (NULL, "/");
//     }

//     printf("%lu\n", sizeof(array));
//     for (i = 0; i < sizeof(array); ++i) 
//         printf("%s\n", array[i]);

//     return 0;
// }