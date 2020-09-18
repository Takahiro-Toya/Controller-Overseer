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

void exPerror(char *msg) {

    perror(msg);
    exit(1);
}

void exSend(int socket_id, void *obj, size_t size, int option) {
    if (send(socket_id, obj, size, option) == -1) {
        exPerror("send");
    }
}

int exRecv(int socket_id, void *obj, size_t size, int option) {
    int numBytes = -1;
    if ((numBytes = recv(socket_id, obj, size, option)) == -1) {
        exPerror("recv");
    }
    return numBytes;
}


