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

/*
 * wrapper of perror() that exits after pringing errno message
 * */
void exPerror(char *msg);

/*
 * wrapper for send() that controls error inside
 * */
void exSend(int socket_id, void *obj, size_t size, int option);

/*
 * wrapper for recv() that controls error inside
 * */
int exRecv(int socket_id, void *obj, size_t size, int option);

/*
 * wrapper for malloc() that controls error inside
 * */
void *exMalloc(size_t size);

#endif