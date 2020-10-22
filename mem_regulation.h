#ifndef __MEM_REGULATION_H
#define __MEM_REGULATION_H

#include "structs.h"

/**
 * Get memory information for pid
 */
unsigned int get_mem_for_pid(pid_t pid);

/**
 * request this function to calculate memory information of process specified by pid and id
 */
void request_add_entry(pid_t pid, int id);

/**
 * Get all memory information stored in the linked list
 */ 
mem_entry_t *get_all_mem_entries();

/**
 * save executable command information in a list
 */ 
int save_request();

/**
 * Clean all saved request information 
 */
void clean_requests();

/**
 * get the head of request informaiton list
 */ 
saved_request_t *get_request_head();

/**
 * get current request id
 */ 
int get_current_id();

/**
 * increment current request id by one (after adding new request to the list)
 */
void increment_current_id();

/**
 * mark the executable request as completed in order to know if the process has terminated or not
 */ 
void mark_request_completed(int id);

/**
 * check if the request specified by id is completed or not
 */ 
bool is_completed(int id);

#endif
