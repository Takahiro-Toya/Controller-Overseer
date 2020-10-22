


// /*
//  * add new pid entry
//  * */ 
// void request_add_entry(pid_t pid, int id);

// mem_entry_t *get_all_mem_entries();

/*
 * add new request to linked list
 * */
void add_request(options_server_t *option);

/*
 * get next request (head of linked list)
 * */
optionContainer_t *get_request();

/*
 * signal handler (for SIGTERM), just siglongjmp to sigsetjmp
 * */
void timeout_handler(int sig);

/*
 * signal handler (for SIGKILL), just siglongjmp to sigsetjmp
 * */
void timeout_handler_f(int sig);

/*
 * execute request
 * */
void handle_request(optionContainer_t *container);

/*
 * manage executions of linked list
 * */
void *handle_requests_loop();

/*
 * init all therad at once
 * */
void init_threads();

/*
 * cancel all threads
 * */
void cancel_all_threads();

/*
 * clean all requests that is awaiting to be excuted
 */
void clean_awaiting_queue();

/*
 * free all request containers stored in the linked list
 */ 
void free_all_requests();

/**
 * free container (like after executing this process)
 */ 
void free_option_container(optionContainer_t *container);