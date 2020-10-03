#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include "mem_regulation.h"
#include "structs.h"

void print_mem_for_pid(pid_t pid) {

    char path_buf[1000];
    char read_buf[2048];
    size_t nread;
    sprintf(path_buf, "/proc/%d/maps", (int)pid);
    // error here
    printf("%s\n", path_buf);
    FILE *file = fopen(path_buf, O_RDONLY);
    // if (file) {
    //     while((nread = fread(read_buf, 1, sizeof(read_buf), file)) > 0) {
    //         fwrite(read_buf, 1, nread, stdout);
    //     }
    //     if (ferror(file)) {
    //         printf("ERRROR\n");
    //     }
    //     fclose(file);
    // } else {
    //     printf("FILE OPEN FAIL\n");
    // }
};