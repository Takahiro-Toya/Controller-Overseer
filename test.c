#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sysinfo.h>


int main(int argc, char* argv[])
{
    
    struct sysinfo si;

    int l = sysinfo(&si);

    printf("%lu %lu\n", si.totalram, si.freeram);
    return 0;
}