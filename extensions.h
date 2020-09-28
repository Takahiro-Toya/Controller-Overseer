#ifndef _EXTENSIONS_H
#define _EXTENSIONS_H

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

void exPerror(char *msg);

void exSend(int socket_id, void *obj, size_t size, int option);

int exRecv(int socket_id, void *obj, size_t size, int option);

void *exMalloc(size_t size);

#endif