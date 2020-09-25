#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "structs.h"


void free_option_container(optionContainer_t *container)
{
    if (container->option->useLog) {
        free(container->option->logfile);
    }
    if (container->option->useOut) {
        free(container->option->outfile);
    }
    free(container->option->execCommand);
    free(container->option);
    free(container);
}