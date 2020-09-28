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

size_t djb_hash(int num);

size_t htab_index(int parent);

parent_pid_t *htab_bucket(int key);

parent_pid_t *htab_find(int key);

bool htab_add(int parent, int child);

void htab_delete(int key);

pid_t get_for(int parent);

void htab_destroy();


#endif