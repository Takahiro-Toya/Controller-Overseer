#define _GNU_SOURCE
#include <stdio.h>   /* standard I/O routines                     */
#include <pthread.h> /* pthread functions and data structures     */
#include <stdlib.h>  /* rand() and srand() functions              */
#include <time.h>
#include <unistd.h>
#include "structs.h"
#include "thread_manage.h"
#include "helper.h"
#include "output_manage.h"
#include "memory_manage.h"

#define NUM_THREADS 5

// thread handling variables
pthread_mutex_t request_mutex;
pthread_cond_t got_request;

// thread trackers
pthread_t threads[NUM_THREADS];
int pending_count = 0;

optionContainer_t *requests = NULL;
optionContainer_t *last_request = NULL;

/**
 * Add new request to the requests queue
 * Critical section inside this function
*/
void add_request(options_server_t *option)
{
    optionContainer_t *container = (optionContainer_t *)malloc(sizeof(optionContainer_t));
    if (!container)
    {
        fprintf(stderr, "add_request: out of memory\n");
        exit(1);
    }

    pthread_mutex_lock(&request_mutex);
    container->option = option;
    container->next = NULL;
    if (option->useOut)
    {
        container->out_fd = get_fd(option->outfile);
    }
    else
    {
        container->out_fd = -1;
    }
    if (option->useLog)
    {
        container->log_fd = get_fd(option->logfile);
    }
    else
    {
        container->log_fd = -1;
    }
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

    pthread_mutex_unlock(&request_mutex);
    pthread_cond_signal(&got_request);
}

/*
 * Get next request (the first element of the linked request)
 */
optionContainer_t *get_request()
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
    return container;
}

/*
 * Execute request
 * It calls fork() 2 times
 *      one for execution 
 *      another one to prevent another thread to share the file descriptors 
 */
void handle_request(optionContainer_t *container)
{
    int outerstatus;
    
    pid_t outer_pid = fork();
    if (outer_pid < 0) {
        fprintf(stderr, "fork");
        exit(1);
    } 
    else if (outer_pid == 0)
    {
        options_server_t *op = container->option;
        if (op != NULL)
        {
            char **args = split_string_by_space(op->execCommand, op->execArgc);
            int retval, status;
            int sstate = 1;
            int sfd[2];
            pipe(sfd);
            // set log redirection
            if (container->log_fd > 0)
            {
                dup2(container->log_fd, 1);
            }

            // spawn for execution
            pid_t pid = fork();

            if (pid < 0)
            {
                fprintf(stderr, "fork");
                exit(1);
            }
            else if (pid == 0) // child process
            {
                close(sfd[0]);
                // set log output
   
                print_log("- attempting to execute %s\n", op->execCommand);

                // set output redirection
                if (container->out_fd > 0)
                {
                    dup2(container->out_fd, 1);
                    dup2(1, 2);
                }
                // if output redirection is not desired, put it back to std
                else if (container->log_fd > 0 && container->out_fd < 0)
                {
                    dup2(get_stdout_copy_fd(), 1);
                    dup2(get_stderr_copy_fd(), 2);
                }
                // execute
                execv(args[0], &args[0]);
                // below ignored if execv succeeded
                sstate = -1;
                write(sfd[1], &sstate, sizeof(sstate));
                if (container->log_fd > 0)
                {
                    dup2(container->log_fd, 1);
                }
                print_log("- could not execute %s\n", op->execCommand);
                close(sfd[1]);
                exit(1);
            }
            else // parent process
            {

                close(sfd[1]);
                if (waitpid(pid, &status, 0) < 0)
                {
                    fprintf(stderr, "waitpid");
                    exit(1);
                }
                if (WIFEXITED(status))
                {
                    read(sfd[0], &sstate, sizeof(sstate));
          
                    if (sstate == 1)
                    {
                        print_log("- %s has been executed with pid %d\n", op->execCommand, pid);
                        print_log("- %d has terminated with status code %d\n", pid, WEXITSTATUS(status));
                    }
                    close(sfd[0]);
                    close(container->out_fd);
                    close(container->log_fd);
                    // need to close stdout stderr fd ?
                    exit(1);
                }
                else
                {
                    print_log("- %d", __LINE__);
                }
            }
        }
    } else {
        waitpid(outer_pid, &outerstatus, 0);
    }
}

/*
 * wait or handle_request in a loop
 */
void *handle_requests_loop()
{

    pthread_mutex_lock(&request_mutex);
    optionContainer_t *container;

    while (1)
    {
        if (pending_count > 0)
        {
            container = get_request();
            if (container)
            {
                pthread_mutex_unlock(&request_mutex);
                handle_request(container);
                free_option_container(container);
                pthread_mutex_lock(&request_mutex);
            }
        }
        else
        {
            pthread_cond_wait(&got_request, &request_mutex);
        }
    }
}

/*
 * Initialise all threads to the number NUM_THREADS at once.
 */
void init_threads()
{
    pthread_mutex_init(&request_mutex, NULL);
    pthread_cond_init(&got_request, NULL);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, handle_requests_loop, NULL);
    }
}

/*
 * Split string by every space and store them into an array of string
 * Specify the number of split (splitnum) so it won't run 'for loop' twice inside
 * returns the result array
 */
char **split_string_by_space(char *string, int splitnum)
{
    char copy[strlen(string)];
    strcpy(copy, string);

    char **strarray = (char **)malloc(sizeof(char *) * (splitnum + 1));
    char *p = strtok(copy, " ");

    for (int i = 0; i < splitnum; i++)
    {
        strarray[i] = (char *)malloc(sizeof(char) * strlen(p));
        strcpy(strarray[i], p);
        p = strtok(NULL, " ");
    }

    free(p);

    strarray[splitnum] = NULL;

    return strarray;
}
