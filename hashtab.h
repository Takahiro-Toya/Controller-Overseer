#ifndef _HASHTAB_H
#define _HASHTAB_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "structs.h"
#include "extensions.h"

void use_htab();

bool htab_init(size_t n);

size_t djb_hash(char *s);

size_t htab_index(char *key);

parent_pid_t *htab_bucket(char *key);

parent_pid_t *htab_find(char *key);

pid_t getpid_for(pid_t parent);

void htab_print();

void item_print(parent_pid_t *i);

bool htab_add(int key, int value);

void htab_delete(int key);

void htab_destroy();

#endif