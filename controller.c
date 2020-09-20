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
#include "helper.h"

#define MAXDATASIZE 100 /* max number of bytes we can get at once */

void send_options(int socket_id, options_t options) {
    
    uint16_t hsize = htons(strlen(options.execCommand));
    uint16_t hargc = htons(options.execArgc);
    // send executable file name size
    exSend(socket_id, &hsize, sizeof(uint16_t), 0);
    // send executable file's argument counts (how many strings the data can be split)
    exSend(socket_id, &hargc, sizeof(uint16_t), 0);
    // send excutable file name 
    exSend(socket_id, options.execCommand, strlen(options.execCommand), 0);

    // send the output file name size
    hsize = options.outfile == NULL ? htons(0) : htons(strlen(options.outfile));
    int size = options.outfile == NULL ? 1 : strlen(options.outfile);
    exSend(socket_id, &hsize, sizeof(uint16_t), 0);
    // send the output file name
    exSend(socket_id, options.outfile == NULL ? "" : options.outfile, size, 0);
    // send the log file name size
    hsize = options.logfile == NULL ? htons(0) : htons(strlen(options.logfile));
    size = options.logfile == NULL ? 1 : strlen(options.logfile);
    exSend(socket_id, &hsize, sizeof(uint16_t), 0);
    exSend(socket_id, options.logfile == NULL ? "" : options.logfile, size, 0);
    // send the -t option boolean (0 for not specified, 1 for specified)
    hsize = options.seconds == -1 ? htons(0) : htons(1);
    uint16_t val = options.seconds == -1 ? htons(0) : htons(options.seconds);
    exSend(socket_id, &hsize, sizeof(uint16_t), 0);
    // send the -t option value
    exSend(socket_id, &val, sizeof(uint16_t), 0);
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
        exit(1);
    }
    if ((he = gethostbyname(argv[1])) == NULL)
    { /* get the host info */
        herror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        exPerror("socket");
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

    send_options(sockfd, options); 

    /* Receive message back from server */
    numbytes = exRecv(sockfd, buf, MAXDATASIZE, 0);

    buf[numbytes] = '\0';

    printf("Received: %s", buf);
    close(sockfd);

    return 0;
}
