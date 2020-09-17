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

void sendOptions(int socket_id, options_t options) {
    
    uint16_t hcmdsize = htons(strlen(options.execCommand));
    // send header
    if (send(socket_id, &hcmdsize, sizeof(uint16_t), 0) == -1) {
        Error("send");
    }
    // send exec command
    if (send(socket_id, options.execCommand, strlen(options.execCommand), 0) == -1) {
        Error("send");
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
        exit(1);
    }
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

    sendOptions(sockfd, options); 

    /* Receive message back from server */
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
    {
        Error("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("Received: %s", buf);
    close(sockfd);

    return 0;
}
