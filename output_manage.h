
void use_fd();

void force_reset();

void set_fd(char *file, int *des);

int get_stdout_copy_fd();

int get_stderr_copy_fd();

// void fd_init(char *outfile, char *logfile);

// void set_to_out();

// void set_to_log();

// void set_to_default();

void print_log(const char *format, ...);

void timestamp();