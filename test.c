#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "usage.h"
#include "extensions.h"
#include "helper.h"

int main () {
    int status;

    printf("Before forking\n");
    pid_t pid = fork();
    if (pid == 0)
    {
        int out_fd;
        int out_fd2;
        if ((out_fd = open("outfile", O_CREAT | O_APPEND | O_WRONLY, 0666)) < 0)
        {
            perror("open outfile");
            exit(1);
        }
        dup2(out_fd, 1);
        printf("I am in child process and should be in outfile\n");
    }
    else 
    {
        if (waitpid(pid, &status, 0) < 0)
        {
            fprintf(stderr, "waitpid\n");
            exit(1);
        }
        printf("parent process\n");
        printf("Where am i ?\n");
    }

    return 0;

}

