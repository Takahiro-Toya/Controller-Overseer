#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include "mem_regulation.h"
#include "structs.h"
#include "extensions.h"
#include "output_manage.h"

#define BUF_SIZE 512

unsigned int get_mem_for_pid(pid_t pid)
{

    unsigned long int count = 0;
    char buf[BUF_SIZE];
    FILE *file;
    sprintf(buf, "/proc/%d/maps", (int)pid);
    file = fopen(buf, "r");
    if (file != NULL) {
        while(fgets(buf, BUF_SIZE, file)){
            unsigned int from, to, offset, dev1, dev2, inode;
            char perms[4];
            int ret = sscanf(buf, "%x-%x %4c %x %x:%x %u", &from, &to, perms, &offset, &dev1, &dev2, &inode);
            
            if (inode == 0) {
                count += (to - from + 1);
            }
        }
    }
    // need to free memory?
    return count;
};

mem_entry_t *mem_head;
void request_add_entry(pid_t pid, int id)
{
    mem_entry_t *new = exMalloc(sizeof(mem_entry_t));
    new->id = id;
    new->pid = pid;
    new->bytes = get_mem_for_pid(pid);
    new->time = get_formatted_time();
    new->next = mem_head;
    mem_head = new;
}

mem_entry_t *get_all_mem_entries() {
    return mem_head;
}

saved_request_t *saved_head;
int current_id = 0;

int save_request(char *file_args)
{
    saved_request_t *new = exMalloc(sizeof(saved_request_t));
    new->completed = false;
    int len = strlen(file_args);
    new->file_args = (char *)exMalloc(sizeof(char) * (len + 1));
    strcpy(new->file_args, file_args);
    new->request_id = current_id;

    new->next = saved_head;
    saved_head = new;

    return current_id;
}

void clean_requests() {
    saved_request_t *toBeDeleted;
    while(saved_head != NULL) {
        toBeDeleted = saved_head;
        saved_head = saved_head->next;
        free(toBeDeleted->file_args);
        free(toBeDeleted);
    }
}

saved_request_t *get_request_head() {
    return saved_head;
}

int get_current_id() {
    return current_id;
}

void increment_current_id() {
    current_id++;
}

void mark_request_completed(int id) {
    for(saved_request_t *r = saved_head; r != NULL; r= r->next) {
        if (r->request_id == id) {
            r->completed = true;
        }
    }
}

bool is_completed(int id) {
    for(saved_request_t *r = saved_head; r != NULL; r= r->next) {
        if (r->request_id == id) {
            return r->completed;
        }
    }
    return false;
}