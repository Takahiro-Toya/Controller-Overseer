#define _GNU_SOURCE
#include <stdio.h>   /* standard I/O routines                     */
#include <pthread.h> /* pthread functions and data structures     */
#include <stdlib.h>  /* rand() and srand() functions              */
#include <time.h>
#include <unistd.h>
#include "structs.h"

#define NUM_THREADS 5

pthread_mutex_t request_mutex;

pthread_cond_t got_request;

int pending_count = 0;

optionContainer_t *requests = NULL;
optionContainer_t *last_request = NULL;

void addRequest(options_t *option, pthread_mutex_t *p_mutex, pthread_cond_t *p_cond_var)
{
    optionContainer_t *container = malloc(sizeof(optionContainer_t));
    if (!container)
    {
        fprintf(stderr, "add_request: out of memory\n");
        exit(1);
    }
    container->option = option;
    container->option = NULL;

    pthread_mutex_lock(p_mutex);

    if (pending_count == 0)
    {
        requests = container;
        last_request = container;
    }
    else
    {
        last_request->next = container;
        last_request = container;
    }

    pending_count++;

    pthread_mutex_unlock(p_mutex);
    pthread_cond_signal(p_cond_var);
}

options_t *get_request()
{
}