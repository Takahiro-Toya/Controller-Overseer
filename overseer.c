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
#include "helper.h"

#define BACKLOG 10 /* how many pending connections queue will hold */

#define RETURNED_ERROR -1

options_t receive_options(int socket_id) {
    options_t op = {1, -1, 0, -1, -1, 1,  NULL, NULL, NULL};
    uint16_t hcmdsize, hargc;
    int numBytes, cmdsize;

    // receive header
    recv(socket_id, &hcmdsize, sizeof(uint16_t), 0);
    recv(socket_id, &hargc, sizeof(uint16_t), 0);
    cmdsize = ntohs(hcmdsize);
    op.execArgc = ntohs(hargc);

    // allocate for data
    op.execCommand = malloc(sizeof(char) * cmdsize);

    // receive data
    numBytes = exRecv(socket_id, op.execCommand, sizeof(char) * cmdsize, 0);
    op.execCommand[numBytes] = '\0';
    return op;
}



int main(int argc, char *argv[])
{
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    socklen_t sin_size;
    int i = 0; // ???

    if (argc != 2)
    {
        fprintf(stderr, "usage: overseer <port>\n");
        exit(1);
    }

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
        timestamp();
        printf("- connection received from %s\n", inet_ntoa(their_addr.sin_addr));

        if (!fork())
        { /* this is the child process */

            options_t op = receive_options(new_fd);
            char **argsarray = split_string_by_space(op.execCommand, op.execArgc);
            char *path = argsarray[0];
            char **args = get_args_from_arg_array(argsarray, op.execArgc - 1);
            int retval, status;
            pid_t pid = fork();
            if (pid < 0) {
                exPerror("fork");
            } else if (pid == 0) {
                // child process
                execv(path, args);
                printf("%s has been executed with pid %d\n", path, getpid());
                exit(retval);
            } else {
                // parent process
                if (waitpid(pid, &status, 0) < 0) {
                    exPerror("waitpid");
                    exit(1);
                }
                if (WIFEXITED(status)) {
                    timestamp();
                    printf("%s has terminated with status code %d\n", path, WEXITSTATUS(status));
                } else {
                    timestamp();
                    printf("i dont know whats happening");
                }
            }

            exSend(new_fd, "Options received\n", 40, 0); 

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