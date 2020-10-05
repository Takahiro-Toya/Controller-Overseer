#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include "setjmp.h"
#include "structs.h"
#include "extensions.h"
#include "output_manage.h"
#include "thread_manage.h"
#include "mem_regulation.h"

#define BACKLOG 10 /* how many pending connections queue will hold */

#define RETURNED_ERROR -1

#define LOGTIME_SIZE 25

saved_request_t *saved_head;
int current_id = 0;

int save_request(char *file_args)
{
    saved_request_t *new = exMalloc(sizeof(saved_request_t));
    int len = strlen(file_args);
    new->file_args = (char *)exMalloc(sizeof(char) * (len + 1));
    strcpy(new->file_args, file_args);
    new->request_id = current_id;
    if (saved_head == NULL)
    {
        saved_head = exMalloc(sizeof(saved_request_t));
        new->next = NULL;
        saved_head = new;
    }
    else
    {
        new->next = saved_head;
        saved_head = new;
    }
    return current_id;
}

options_server_t *receive_options(int socket_id)
{
    options_server_t *op = (options_server_t *)exMalloc(sizeof(options_server_t));

    op->type = FileExec;
    op->request_id = -1;
    op->mempid = -1;
    op->memkill = -1;
    op->execArgc = -1;
    op->seconds = -1;
    op->useOut = false;
    op->useLog = false;

    uint16_t recved;

    // receive headers
    // receive option type
    exRecv(socket_id, &recved, sizeof(uint16_t), 0);
    op->type = ntohs(recved);

    if (op->type == Mem)
    {
        return op;
    }
    else if (op->type == MemWithPid)
    {
        exRecv(socket_id, &recved, sizeof(uint16_t), 0);
        op->mempid = ntohs(recved);
        return op;
    }
    else if (op->type == Memkill)
    {
        exRecv(socket_id, &recved, sizeof(uint16_t), 0);
        op->memkill = ntohs(recved);
        return op;
    }
    else if (op->type == FileExec)
    {
        int numBytes, hsizeint, tbool;
        char *discard = (char *)exMalloc(sizeof(char) * 2);

        exRecv(socket_id, &recved, sizeof(uint16_t), 0);
        hsizeint = ntohs(recved);
        // malloc for filename
        op->execCommand = (char *)exMalloc(sizeof(char) * hsizeint);
        // receive number of arguments
        exRecv(socket_id, &recved, sizeof(uint16_t), 0);
        op->execArgc = ntohs(recved);
        // receive commands
        numBytes = exRecv(socket_id, op->execCommand, sizeof(char) * hsizeint, 0);
        op->execCommand[numBytes] = '\0';

        // receive -o header
        exRecv(socket_id, &recved, sizeof(uint16_t), 0);
        hsizeint = ntohs(recved);
        // receive o file name
        if (hsizeint != 0)
        {
            op->useOut = true;
            op->outfile = (char *)exMalloc(sizeof(char) * hsizeint);
            numBytes = exRecv(socket_id, op->outfile, sizeof(char) * hsizeint, 0);
            op->outfile[numBytes] = '\0';
        }
        else
        {
            exRecv(socket_id, discard, sizeof(char), 0);
        }
        // receive -log header
        exRecv(socket_id, &recved, sizeof(uint16_t), 0);
        hsizeint = ntohs(recved);
        // receive log file name
        if (hsizeint != 0)
        {
            op->useLog = true;
            op->logfile = malloc(sizeof(char) * hsizeint);
            numBytes = exRecv(socket_id, op->logfile, sizeof(char) * hsizeint, 0);
            op->logfile[numBytes] = '\0';
        }
        else
        {
            exRecv(socket_id, discard, sizeof(char), 0);
        }
        // receive -t header
        exRecv(socket_id, &recved, sizeof(uint16_t), 0);
        tbool = ntohs(recved);
        // receive -t val
        if (tbool != 0)
        {
            exRecv(socket_id, &recved, sizeof(uint16_t), 0);
            op->seconds = ntohs(recved);
        }
        else
        {
            exRecv(socket_id, &recved, sizeof(uint16_t), 0);
        }
        free(discard);
        return op;
    }
    else
    {
        fprintf(stderr, "Undefined option type");
        exit(EXIT_FAILURE);
        return op;
    }
}

static jmp_buf env;
void sigint_handler_o(int sig)
{
    siglongjmp(env, 3);
}

int main(int argc, char *argv[])
{
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    socklen_t sin_size;

    if (argc != 2)
    {
        fprintf(stderr, "usage: overseer <port>\n");
        exit(1);
    }

    init_threads();

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        exPerror("socket");
    }

    /* Enable address/port reuse, useful for server development */
    int opt_enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt_enable, sizeof(opt_enable));

    /* clear address struct */
    memset(&my_addr, 0, sizeof(my_addr));

    /* generate the end point */
    my_addr.sin_family = AF_INET;            /* host byte order */
    my_addr.sin_port = htons(atoi(argv[1])); /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY;    /* auto-fill with my IP */

    /* bind the socket to the end point */
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        exPerror("bind");
    }

    /* start listnening */
    if (listen(sockfd, BACKLOG) == -1)
    {
        exPerror("listen");
    }

    struct sigaction sa;
    memset(&sa, '\0', sizeof(char));
    sa.sa_handler = &sigint_handler_o;
    sigaction(SIGINT, &sa, NULL);

    use_fd();

    /* repeat: accept, send, close the connection */
    /* for every accepted connection, use a sepetate process or thread to serve it */
    while (1)
    { /* main accept() loop */

        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
                             &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }
        if (sigsetjmp(env, 3) != 0)
        {
            close(new_fd);
            break;
        }

        print_log("- Connection received from %s\n", inet_ntoa(their_addr.sin_addr));

        options_server_t *op = receive_options(new_fd);

        if (op->type == Mem)
        {
            print_log(" - Got request 'mem' without pid\n", op->mempid);
            uint16_t num_entries = htons(current_id);
            // send number of entries to the server first
            exSend(new_fd, &num_entries, sizeof(uint16_t), 0);
            printf("Entry count = %d\n", current_id);
            mem_entry_t *all = get_all_mem_entries();
            for (saved_request_t *r = saved_head; r != NULL; r = r->next)
            {
                for (mem_entry_t *e = all; e != NULL; e = e->next)
                {
                    if (r->request_id == e->id)
                    {
                        // send the last record of each execution
                        // send pid
                        uint16_t ndata = htons((int)(e->pid));
                        exSend(new_fd, &ndata, sizeof(uint16_t), 0);
                        // send bytes
                        uint32_t ndata_b = htonl(e->bytes);
                        exSend(new_fd, &ndata_b, sizeof(uint32_t), 0);
                        // // send file args string length 
                        ndata = htons(strlen(r->file_args));
                        exSend(new_fd, &ndata, sizeof(uint16_t), 0);
                        // send string
                        exSend(new_fd, r->file_args, strlen(r->file_args), 0);
                        printf("%d %d %s\n", e->pid, e->bytes, r->file_args);
                        break;
                    }
                }
            }
            close(new_fd);
        }
        else if (op->type == MemWithPid)
        {
            print_log(" - Got request 'mem' with pid\n", op->mempid);
            mem_entry_t *all = get_all_mem_entries();
            int count = 0;
            // count number of entries for pid
            for (mem_entry_t *e = all; e != NULL; e = e->next)
            {
                if (e->pid == op->mempid)
                {
                    count++;
                }
            }
            uint16_t num_entries = htons(count);
            // send number of entries
            exSend(new_fd, &num_entries, sizeof(uint16_t), 0);
            for (mem_entry_t *e = all; e != NULL; e = e->next)
            {
                if (e->pid == op->mempid)
                {
                    exSend(new_fd, e->time, LOGTIME_SIZE, 0);
                    uint32_t nbytes = htonl(e->bytes);
                    exSend(new_fd, &nbytes, sizeof(uint32_t), 0);
                }
            }
            close(new_fd);
        }
        else if (op->type == Memkill)
        {
            print_log(" - Memkill %d\n", op->memkill);
        }
        else if (op->type == FileExec)
        {
            int id = save_request(op->execCommand);
            op->request_id = id;
            current_id++;
            add_request(op);
            close(new_fd);
        }
        else
        {
            close(new_fd);
            fprintf(stderr, "Undefined option type");
            exit(EXIT_FAILURE);
        }
    } // while loop
}