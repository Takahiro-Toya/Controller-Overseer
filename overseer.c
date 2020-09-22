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
#include "output_manage.h"

#define BACKLOG 10 /* how many pending connections queue will hold */

#define RETURNED_ERROR -1

options_t receive_options(int socket_id)
{
    options_t op = {1, -1, -1, -1, 1, NULL, NULL, NULL};
    uint16_t hsize, hargc;
    int numBytes, hsizeint, tbool;

    char *discard = (char *)malloc(sizeof(char) * 2);

    // receive file name header
    exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
    hsizeint = ntohs(hsize);
    // malloc for filename
    op.execCommand = malloc(sizeof(char) * hsizeint);
    // receive number of arguments
    exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
    op.execArgc = ntohs(hsize);
    // receive file name
    numBytes = exRecv(socket_id, op.execCommand, sizeof(char) * hsizeint, 0);
    op.execCommand[numBytes] = '\0';

    // receive -o header
    exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
    hsizeint = ntohs(hsize);
    // receive o file name
    if (hsizeint != 0)
    {
        op.outfile = malloc(sizeof(char) * hsizeint);
        numBytes = exRecv(socket_id, op.outfile, sizeof(char) * hsizeint, 0);
        op.outfile[numBytes] = '\0';
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
        op.logfile = malloc(sizeof(char) * hsizeint);
        numBytes = exRecv(socket_id, op.logfile, sizeof(char) * hsizeint, 0);
        op.logfile[numBytes] = '\0';
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
        op.seconds = ntohs(hsize);
    }
    else
    {
        exRecv(socket_id, &hsize, sizeof(uint16_t), 0);
    }
    return op;
}

int main(int argc, char *argv[])
{
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    socklen_t sin_size;
    // int i = 0; // ???

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
        
        if (!fork())
        {

            options_t op = receive_options(new_fd);
            fd_init(op.outfile, op.logfile);
            char **args = split_string_by_space(op.execCommand, op.execArgc);
            int retval, status;
            int sstate = 1;
            int sfd[2];
            pipe(sfd);
            print_log("- Connection received from %s\n", inet_ntoa(their_addr.sin_addr));
            // spawn for executing files
            pid_t pid = fork();
            if (pid < 0)
            {
                exPerror("fork");
            }
            else if (pid == 0) // child process
            {
                close(sfd[0]);
                set_to_log();
                print_log("- attempting to execute %s\n", op.execCommand);
                
                // execute
                set_to_out();
                execv(args[0], &args[0]);
                // below ignored if execv succeeded
                sstate = -1;
                write(sfd[1], &sstate, sizeof(sstate));
                set_to_log();
                print_log("- could not execute %s\n", op.execCommand);
                close(sfd[1]);
            }
            else // parent process
            {
                close(sfd[1]);
                if (waitpid(pid, &status, 0) < 0)
                {
                    exPerror("waitpid");
                }
                if (WIFEXITED(status))
                {
                    set_to_log();
                    read(sfd[0], &sstate, sizeof(sstate));
                    if (sstate == 1)
                    {
                        print_log("- %s has been executed with pid %d\n", op.execCommand, pid);
                        print_log("- %d has terminated with status code %d\n", pid, WEXITSTATUS(status));
                    }
                    close(sfd[0]);
                }
                else
                {
                    print_log("- %d", __LINE__);
                }
            }
            set_to_default();
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