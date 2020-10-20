#define _GNU_SOURCE
#include <stdio.h>   /* standard I/O routines                     */
#include <pthread.h> /* pthread functions and data structures     */
#include <stdlib.h>  /* rand() and srand() functions              */
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> // linux
#include <sys/wait.h>  // linux
#include "structs.h"
#include "extensions.h"
#include "thread_manage.h"
#include "helper.h"
#include "output_manage.h"
#include "memory_manage.h"
#include "setjmp.h"
#include "mem_regulation.h"

#define NUM_THREADS 5
#define SIGKILL_THRESHOLD 5

// thread handling variables
pthread_mutex_t request_mutex;
pthread_cond_t got_request;

pthread_mutex_t mem_request_mutex;
pthread_mutex_t save_request_mutex;

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
    optionContainer_t *container = (optionContainer_t *)exMalloc(sizeof(optionContainer_t));
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

static jmp_buf env;
static jmp_buf env2;

/*
 * Execute request with two processes
 */
void handle_request(optionContainer_t *container)
{
    int outerstatus;
    // good to have outer fork here to avoid sharing of file descriptors
    // with other threads (processes)
    int fd_execpid[2];
    pipe(fd_execpid);
    pid_t outer_pid = fork();
    if (outer_pid < 0)
    {
        exPerror("fork");
    }
    else if (outer_pid == 0)
    {
        close(fd_execpid[0]);
        options_server_t *op = container->option;
        if (op != NULL)
        {
            char **args = split_string_by_space(op->execCommand, op->execArgc);
            int status;
            int sstate = 1; // success status of the exec program
            int sfd[2];     // pipe for sstate
            pipe(sfd);
            // set log redirection
            if (container->log_fd > 0)
            {
                dup2(container->log_fd, 1);
            }
            // spawn for execution
            pid_t exec_pid = fork();
            if (exec_pid < 0)
            {
                exPerror("fork");
            }
            else if (exec_pid == 0) // child process -> replaced by exec
            {
                close(sfd[0]);

                print_log("- attempting to execute %s\n", op->execCommand);

                // set output redirection
                if (container->out_fd > 0)
                {
                    dup2(container->out_fd, 1);
                    dup2(1, 2);
                }
                // if output redirection is not desired, but due to log output,
                // put it back to std
                else if (container->log_fd > 0 && container->out_fd < 0)
                {
                    force_reset();
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
                // send child procedd pid to outer parent (same process as overseer)
                write(fd_execpid[1], &exec_pid, sizeof(pid_t));
                close(sfd[1]);
                close(fd_execpid[1]);
                alarm(container->option->seconds == -1 ? 10 : container->option->seconds);
                signal(SIGALRM, timeout_handler);
                int terminate_success = 0;
                if (sigsetjmp(env, 1) != 0)
                {
                    alarm(0);
                    signal(SIGALRM, SIG_DFL);
                    kill(exec_pid, SIGTERM);
                    print_log("- sent SIGTERM to %d\n", exec_pid);
                    alarm(SIGKILL_THRESHOLD);
                    signal(SIGALRM, timeout_handler_f);
                    if (sigsetjmp(env2, 1) != 0)
                    {
                        alarm(0);
                        signal(SIGALRM, SIG_DFL);
                        kill(exec_pid, SIGKILL);
                        print_log("- sent SIGKILL detachto %d\n", exec_pid);
                    }
                }

                // wait for execution in child process
                if (waitpid(exec_pid, &status, 0) < 0)
                {
                    exPerror("wait");
                }

                // exited
                if (WIFEXITED(status))
                {
                    read(sfd[0], &sstate, sizeof(sstate));

                    if (sstate == 1)
                    {
                        print_log("- %s has been executed with pid %d\n", op->execCommand, exec_pid);
                        print_log("- %d has terminated with status code %d\n", exec_pid, WEXITSTATUS(status));
                    }
                }
                // signaled and terminated
                if (WIFSIGNALED(status))
                {
                    if (WTERMSIG(status) == SIGTERM)
                    {
                        print_log("- %d has terminated with status code %d\n", exec_pid, WEXITSTATUS(status));
                    }
                }
                alarm(0);
                signal(SIGALRM, SIG_DFL);
                close(sfd[0]);
                // close descriptor
                close_std_copy();
                if (container->log_fd > 0)
                {
                    close(container->log_fd);
                }
                if (container->out_fd > 0)
                {
                    close(container->out_fd);
                }
                exit(EXIT_SUCCESS);
            }
        }
    }
    // outer fork
    else
    {
        close(fd_execpid[1]);
        int target_pid = -1;
        int wait;
        while ((wait = waitpid(outer_pid, &outerstatus, WNOHANG)) == 0)
        {
            // read only once
            if (target_pid == -1)
            {
                read(fd_execpid[0], &target_pid, sizeof(pid_t));
                close(fd_execpid[0]);
                // append memory emtry every one second
            }
            else
            {
                sleep(1);
                pthread_mutex_lock(&mem_request_mutex);
                request_add_entry(target_pid, container->option->request_id);
                pthread_mutex_unlock(&mem_request_mutex);
            }
        }
        if (wait < 0)
        {
            exPerror("waitpid");
        }
        if (WIFEXITED(outerstatus))
        {
            mark_request_completed(container->option->request_id);
        }
    }
}

void timeout_handler(int sig)
{
    siglongjmp(env, 1);
}

void timeout_handler_f(int sig)
{
    siglongjmp(env2, 1);
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

void cancel_all_threads()   
{   
    clean_requests();
    free_all_requests(requests);
    free(last_request);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_detach(threads[i]);
    }

}


