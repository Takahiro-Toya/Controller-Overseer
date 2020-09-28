
#include <inttypes.h> // for portable integer declarations
#include <stdbool.h>  // for bool type
#include <stdio.h>    // for printf()
#include <stdlib.h>   // for malloc(), free(), NULL
#include <string.h>   // for strcmp()
#include "structs.h"
#include "hashtab.h"

htab_t *h;

void use_htab()
{
    if (h == NULL)
    {
        h = (htab_t *)malloc(sizeof(htab_t));
        if (h == NULL)
        {
            exit(1);
        }
    }
    htab_init(10);
}

bool htab_init(size_t n)
{
    h->size = n;
    h->buckets = (parent_pid_t **)calloc(n, sizeof(parent_pid_t *));
    return h->buckets != 0;
}

size_t djb_hash(char *s)
{
    size_t hash = 5381;
    int c;
    while ((c = *s++) != '\0')
    {
        // hash = hash * 33 + c
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

size_t htab_index(char *key)
{
    return djb_hash(key) % h->size;
}

parent_pid_t *htab_bucket(char *key)
{
    return h->buckets[htab_index(key)];
}

parent_pid_t *htab_find(char *key)
{
    for (parent_pid_t *i = htab_bucket(key); i != NULL; i = i->next)
    {
        printf("find %d %s\n", __LINE__, i->key);
        if (strcmp(i->key, key) == 0)
        { // found the key
            return i;
        }
    }
    return NULL;
}

int getpid_for(pid_t parent) {
    char buf[20];
    sprintf(buf, "%d", (int)parent);
    parent_pid_t *f = htab_find(buf);
    if (f == NULL) {
        return -1;
    } else {
        return f->value;
    }
}

void htab_print()
{
    printf("hash table with %d buckets\n", h->size);
    for (size_t i = 0; i < h->size; ++i)
    {
        printf("bucket %d: ", i);
        if (h->buckets[i] == NULL)
        {
            printf("empty\n");
        }
        else
        {
            for (parent_pid_t *j = h->buckets[i]; j != NULL; j = j->next)
            {
                item_print(j);
                if (j->next != NULL)
                {
                    printf(" -> ");
                }
            }
            printf("\n");
        }
    }
}

void item_print(parent_pid_t *i)
{
    printf("key=%s value=%d", i->key, i->value);
}

bool htab_add(pid_t key, pid_t value)
{
    char buf[20];
    sprintf(buf, "%d", (int)key);
    // allocate new item
    parent_pid_t *newhead = (parent_pid_t *)malloc(sizeof(parent_pid_t));
    if (newhead == NULL)
    {
        // need to change 
        printf("malloc failed at %d\n", __LINE__);
        return false;
    }
    newhead->key = buf;
    newhead->value = (int)value;
    // hash key and place item in appropriate bucket
    size_t bucket = htab_index(buf);
    newhead->next = h->buckets[bucket];
    h->buckets[bucket] = newhead;
    return true;
}



void htab_delete(int key)
{
    char buf[20];
    sprintf(buf, "%d", key);
    parent_pid_t *head = htab_bucket(buf);
    parent_pid_t *current = head;
    parent_pid_t *previous = NULL;
    while (current != NULL)
    {
        if (strcmp(current->key, buf) == 0)
        {
            if (previous == NULL)
            { // first item in list
                h->buckets[htab_index(buf)] = current->next;
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

void htab_destroy(htab_t *h)
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
