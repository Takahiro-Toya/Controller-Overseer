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

int main()
{
    int status;
    
    pid_t f = fork();
    use_htab();
    if (f == 0) {
        
        
    } else {
        htab_add(10, 11);
        htab_add(12, 13);
        htab_add(14, 15);
        htab_add(16, 17);
        htab_add(18, 19);
        htab_add(20, 21);
        htab_print();
        waitpid(f, &status, 0);
        printf("found %d\n", getpid_for(10));
        printf("found %d\n", getpid_for(18));
       
    }
    



    return 0;
}
