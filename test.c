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

typedef struct {
    int num;
    char *c;
} hello;

int main () {

    hello *h = malloc(sizeof(hello));
    
    h->num = 4;
    h->c = "HEY";

    printf("%s\n", (char *)h);

    return 0;
}
