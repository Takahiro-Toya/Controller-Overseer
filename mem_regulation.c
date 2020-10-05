#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include "mem_regulation.h"
#include "structs.h"
#include "extensions.h"
#include "output_manage.h"

unsigned int get_mem_for_pid(pid_t pid)
{

    unsigned int count = 0;
    count = 500000;
    // char buf[512];
    // FILE *file;
    // sprintf(buf, "/proc/%d/maps", (int)pid);
    // file = fopen(buf, "r");
    // while(fgets(buf, 512, file)){
    //     unsigned int from, to, perms, offset, dev, inode, pathname;
    //     int ret = sscanf(buf, "%x-%x %4c %x %x %u %s", &from, &to, &perms, &offset, &dev, &inode, &pathname);
    //     if (ret != 10) {
    //         break;
    //     }
    //     if (inode == 0) {
    //         count += (to - from);
    //     }
    // }
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
    if (mem_head == NULL) {
        // this entry will be at the trailing 
        mem_head = exMalloc(sizeof(mem_entry_t));
        new->next = NULL;
        mem_head = new;
    } else {
        new->next = mem_head;
        mem_head = new;
    }
}

mem_entry_t *get_all_mem_entries() {
    return mem_head;
}