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

#define BACKLOG 10 /* how many pending connections queue will hold */

#define RETURNED_ERROR -1

options_server_t *receive_options(int socket_id)
{

    options_server_t *op = (options_server_t *)exMalloc(sizeof(options_server_t));
    op->mem = -1;
    op->memkill = -1;
    op->execArgc = -1;
    op->seconds = -1;
    op->useOut = false;
    op->useLog = false;
    
   uint16_t hsize, hargc;
    int numBytes, hsizeint, tbool;

    char *discard = (char *)exMalloc(sizeof(char) * 2);

    // receive file name header
    exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
    hsizeint = ntohs(hsize);
    // malloc for filename
    op->execCommand = (char *)exMalloc(sizeof(char) * hsizeint);
    // receive number of arguments
    exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
    op->execArgc = ntohs(hsize);
    // receive file name
    numBytes = exRecv(socket_id, op->execCommand, sizeof(char) * hsizeint, 0);
    op->execCommand[numBytes] = '\0';

    // receive -o header
    exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
    hsizeint = ntohs(hsize);
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
    exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
    hsizeint = ntohs(hsize);
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
    exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
    tbool = ntohs(hsize);
    // receive -t val
    if (tbool != 0)
    {
        exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
        op->seconds = ntohs(hsize);
    }
    else
    {
        exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
    }
    free(discard);
    return op;
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
            printf("%d\n", __LINE__);
            break;
        }
        
        print_log("- Connection received from %s\n", inet_ntoa(their_addr.sin_addr));
        options_server_t *op = receive_options(new_fd);
        close(new_fd);
        add_request(op);
        
    }
}