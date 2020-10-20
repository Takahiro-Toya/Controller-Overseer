#ifndef _MEMORY_MANAGEMENT_H
#define _MEMORY_MANAGEMENT_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "structs.h"


/*
 * free memory used by <argument>
 * */
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

/*
 * free memory of all optionContainer_t from head to last of linked list
 * */
void free_all_requests(optionContainer_t *requests) {
    optionContainer_t *toBeDeleted;
    while (requests != NULL) {
        toBeDeleted = requests;
        requests = requests->next;
        free_option_container(toBeDeleted);
    }
}

#endif

