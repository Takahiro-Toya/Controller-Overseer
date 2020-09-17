#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "usage.h"
#include "extensions.h"

#define MAXDATASIZE 100 /* max number of bytes we can get at once */

#define ARRAY_SIZE 30

void sendExecArguments(int socket_id, char *filename, char **args, int argsCount)
{
    uint16_t fnamelen = htons(strlen(filename));
    uint16_t argsc = htons(argsCount);
    // send file name length
    if (send(socket_id, &fnamelen, sizeof(uint16_t), 0) == -1)
    {
        Error("send");
    }
    // send the number of arguments
    if (send(socket_id, &argsc, sizeof(uint16_t), 0) == -1) 
    {
        Error("send");
    }
    // send the file name
    if (send(socket_id, filename, strlen(filename), 0) == -1)
    {
        Error("send");
    }
    // send args
    for (int i = 0; i < argsCount * 2; i++)
    {
        if (i % 2 == 0) {
            uint16_t arglen = htons(strlen(args[i]));
            if (send(socket_id, &arglen, sizeof(uint16_t), 0) == -1) {
                Error("send");
            }
        } else {
            if (send(socket_id, args[i], strlen(args[i]), 0) == -1) {
                Error("send");
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes, i = 0;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr; /* connector's address information */

    options_t options = getControllerInitCommand(argc, argv);
    if (options.success == -1)
    {
        fprintf(stderr, "*** Args error ***\n");
        exit(1);
    }
    // } else {
    //     printf("log: %s, out: %s, seconds: %d, file: %s, fileargs: %d, mem: %d, memkill: %d\n",
    //         options.logfile, options.outfile, options.seconds, options.filename, options.fileargscount, options.mem, options.memkill
    //     );
    // }
    if ((he = gethostbyname(argv[1])) == NULL)
    { /* get the host info */
        herror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        Error("socket");
    }

    /* clear address struct */
    memset(&their_addr, 0, sizeof(their_addr));

    their_addr.sin_family = AF_INET;            /* host byte order */
    their_addr.sin_port = htons(atoi(argv[2])); /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);

    if (connect(sockfd, (struct sockaddr *)&their_addr,
                sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Could not connect to overseer at %s %s\n", argv[1], argv[2]);
        exit(1);
    }

    sendExecArguments(sockfd, options.filename, options.fileargs, options.fileargscount);

    /* Receive message back from server */
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
    {
        Error("recv");
    }

    buf[numbytes] = '\0';

    buf[numbytes] = '\0';

    printf("Received: %s", buf);
    close(sockfd);

    return 0;
}
