
/*
 * to declare that the program is going to use file descriptor 
 * This function saves stdout and stderr descriptor so that they can be
 * restored when necessary
 * */
void use_fd();

/*
 * reset output destination to stdout stderr using saved descriptor
 * which was created through use_fd() method
 * */
void force_reset();

/*
 * close copy of stdout/stderr descriptor which was created by use_fd()
 * */
void close_std_copy();

/*
 * open or create file named <argument> then returns file descriptor
 * */
int get_fd(char *file);

/*
 * to make things easier to print time stamp and message
 * */
void print_log(const char *format, ...);

/*
 * print current time
 * */
void timestamp();

char *get_formatted_time();