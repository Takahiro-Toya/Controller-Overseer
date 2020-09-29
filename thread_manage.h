void add_request(options_server_t *option);

optionContainer_t *get_request();

void timeout_handler(int sig);

void timeout_handler_f(int sig);

void sigint_handler(int sig);

void handle_request(optionContainer_t *container);

void *handle_requests_loop();

void init_threads();

void cancel_all_threads();

char **split_string_by_space(char *string, int splitnum);