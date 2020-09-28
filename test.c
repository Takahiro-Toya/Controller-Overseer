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
#include "hashtab.h"

int main()
{

    use_htab();
    
    htab_add((pid_t)10, (pid_t)11);
    htab_add((pid_t)12, (pid_t)13);
    htab_add((pid_t)14, (pid_t)15);
    htab_add((pid_t)16, (pid_t)17);
    htab_add((pid_t)18, (pid_t)19);
    htab_add((pid_t)20, (pid_t)21);

    printf("found %d\n", getpid_for((pid_t)10));
    printf("found %d\n", getpid_for((pid_t)18));

    return 0;
}
