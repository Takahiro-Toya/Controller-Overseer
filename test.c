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
    int dfd = dup(1);
    print_int(dfd);
    int fd = open("logfile", O_CREAT | O_APPEND | O_WRONLY, 0666);
    dup2(fd, 1);
    for (int i = 0; i < 5; i++) {
        printf("%s\n", result[i]);
    }

    dup2(dfd, 1);
    printf("where am i");

    return 0;

}

