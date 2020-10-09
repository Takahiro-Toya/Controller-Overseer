#ifndef __MEM_REGULATION_H
#define __MEM_REGULATION_H

#include "structs.h"

unsigned int get_mem_for_pid(pid_t pid);

void request_add_entry(pid_t pid, int id);

mem_entry_t *get_all_mem_entries();

int save_request();

saved_request_t *get_request_head();

int get_current_id();

void increment_current_id();

void mark_request_completed(int id);

bool is_completed(int id);

#endif