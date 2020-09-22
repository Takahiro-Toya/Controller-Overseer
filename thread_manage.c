#define _GNU_SOURCE
#include <stdio.h>   /* standard I/O routines                     */
#include <pthread.h> /* pthread functions and data structures     */
#include <stdlib.h>  /* rand() and srand() functions              */
#include <time.h>
#include <unistd.h>
#include "structs.h"

#define NUM_THREADS 5

// thread handling variables
pthread_mutex_t request_mutex;
pthread_cond_t got_request;

// thread trackers
pthread_t threads[NUM_THREADS];
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
    optionContainer_t *container;
    
    if (pending_count > 0) 
    {
        container = requests;
        requests = container->next;
        if (requests == NULL)
        {
            last_request = NULL;
        } 
        pending_count--;
    } 
    else 
    {
        container = NULL;
    }
    return container->option;
}

void handle_request(options_t *option) {

}

void *handle_requests_loop()
{
    options_t *option;

    pthread_mutex_lock(&request_mutex);

    while(1)
    {
        if (pending_count > 0)
        {
            option = get_request();
            if (option)
            {
                pthread_mutex_unlock(&request_mutex);
                handle_request(option);
                free(option);
                pthread_mutex_lock(&request_mutex);
            }
        }
        else 
        {
            pthraed_cond_wait(&got_request, &request_mutex);
        }
    }
}

void init_threads()
{
    pthread_mutex_init(&request_mutex, NULL);
    pthread_cond_init(&got_request, NULL);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, handle_requests_loop, NULL);
    }
}