#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "structs.h"
#include "extensions.h"
#include "hashtab.h"

htab_t *h;

void use_htab() {
    size_t buckets = 10;
    if (!htab_init(buckets))
    {
        printf("failed to initialise hash table\n");
        exit(-1);
    }
}

bool htab_init(size_t n)
{
    if (h == NULL)
    {
        h = (htab_t *)malloc(sizeof(htab_t));
        if (h == NULL) {
            return false;
        }
    }
    h->size = n;
    h->buckets = (parent_pid_t **)calloc(n, sizeof(parent_pid_t *));
    return h->buckets != 0;
}

size_t djb_hash(int num)
{
    size_t hash = 5381;
    int c;
    while (c != num++)
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

size_t htab_index(int parent)
{
    return djb_hash(parent) % h->size;
}

parent_pid_t *htab_bucket(int key)
{
    return h->buckets[htab_index(key)];
}

parent_pid_t *htab_find(int key)
{
    for (parent_pid_t *i = htab_bucket(key); i != NULL; i = i->next)
    {
        if (i->parent == key)
        { // found the key
            return i;
        }
    }
    return NULL;
}

bool htab_add(int parent, int child)
{
    // allocate new item
    parent_pid_t *newhead = (parent_pid_t *)malloc(sizeof(parent_pid_t));
    if (newhead == NULL)
    {
        return false;
    }
    newhead->parent = parent;
    newhead->child = child;

    // hash key and place item in appropriate bucket
    size_t bucket = htab_index(parent);
    newhead->next = h->buckets[bucket];
    h->buckets[bucket] = newhead;
    return true;
}

void htab_delete(int key)
{
    parent_pid_t *head = htab_bucket(key);
    parent_pid_t *current = head;
    parent_pid_t *previous = NULL;
    while (current != NULL)
    {
        if (current->parent == key)
        {
            if (previous == NULL)
            { // first item in list
                h->buckets[htab_index(key)] = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current);
            break;
        }
        previous = current;
        current = current->next;
    }
}

pid_t get_for(int parent) {
    parent_pid_t *tab = htab_find(parent);
    return tab == NULL ? (pid_t)tab->child : -1;
}

void htab_destroy()
{
    // free linked lists
    for (size_t i = 0; i < h->size; ++i)
    {
        parent_pid_t *bucket = h->buckets[i];
        while (bucket != NULL)
        {
            parent_pid_t *next = bucket->next;
            free(bucket);
            bucket = next;
        }
    }

    // free buckets array
    free(h->buckets);
    h->buckets = NULL;
    h->size = 0;
}

