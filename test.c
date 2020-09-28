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
    
    htab_add(10, 11);
    htab_add(12, 13);
    htab_add(14, 15);
    htab_add(16, 17);
    htab_add(18, 19);
    htab_add(20, 21);

    printf("%d\n", get_for(10));
    printf("%d\n", get_for(18));

    htab_delete(14);
    printf("%d\n", get_for(14));
    return 0;
}
