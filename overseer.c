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
#include "structs.h"
#include "extensions.h"

#define ARRAY_SIZE 30

#define BACKLOG 10 /* how many pending connections queue will hold */

#define RETURNED_ERROR -1

#define PORT_NO 54321

options_t receiveExecArguments(int socket_id)
{

    options_t op = {1, -1, 0, -1, -1, NULL, NULL, NULL, NULL};
    uint16_t fnamelen;
    uint16_t argsc;
    // receive file name length
    if (recv(socket_id, &fnamelen, sizeof(uint16_t), 0) == -1)
    {
        Error("recv");
    }
    op.filename = (char *)malloc(sizeof(ntohs(fnamelen)) + 1);
    // receive args count
    if (recv(socket_id, &argsc, sizeof(uint16_t), 0) == -1)
    {
        Error("recv");
    }
    op.fileargscount = ntohs(argsc);
    // receive file name
    if (recv(socket_id, op.filename, sizeof(ntohs(fnamelen)), 0) == -1)
    {
        Error("recv");
    }
    op.filename[ntohs(fnamelen)] = '\0';
    // receive args
    op.fileargs = (char **)malloc(sizeof(ntohs(argsc)));
    uint16_t len;
    for (int i = 0; i < ntohs(argsc) * 2; i++)
    {
        if (i % 2 == 0) {
            if (recv(socket_id, &len, sizeof(uint16_t), 0) == -1) {
                Error("recv");
            }
            op.fileargs[i] = (char *)malloc(sizeof(ntohs(len)) + 1);
        } else {
            if (recv(socket_id, op.fileargs[i], sizeof(ntohs(len)), 0) == -1) {
                Error("recv");
            }
        }
    }

    return op;
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

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        Error("socket");
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
        Error("bind");
    }

    /* start listnening */
    if (listen(sockfd, BACKLOG) == -1)
    {
        Error("listen");
    }

    /* repeat: accept, send, close the connection */
    /* for every accepted connection, use a sepetate process or thread to serve it */
    while (1)
    { /* main accept() loop */
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
                             &sin_size)) == -1)
        {
            Error("accept");
        }
        time_t t = time(NULL);
        struct tm *local = localtime(&t);
        printf("%d-%d-%d %d:%d:%d - connection received from %s\n", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec, inet_ntoa(their_addr.sin_addr));
        options_t op = receiveExecArguments(sockfd);
        printf("log: %s, out: %s, seconds: %d, file: %s, fileargs: %d, mem: %d, memkill: %d\n",
            op.logfile, op.outfile, op.seconds, op.filename, op.fileargscount, op.mem, op.memkill
        );

        if (!fork())
        { /* this is the child process */

            if (send(new_fd, "All of array data received by server\n", 40, 0) == -1)
                Error("send");
            close(new_fd);
            exit(0);
        }
        else
        {
            close(new_fd); /* parent doesn't need this */
        }
        while (waitpid(-1, NULL, WNOHANG) > 0)
            ; /* clean up child processes */
    }
}