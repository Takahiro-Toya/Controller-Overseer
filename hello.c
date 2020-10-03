#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>



int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        fprintf(stderr, "usage : ./hello <name> ...\n");
        return 1;
    }

    if (strcmp(argv[1], "taka") == 0) {
        signal(SIGTERM, SIG_IGN);
    } 



    
    for (int i = 1; i < argc ; i++) {
        
        printf("Hello, %s. my PID: %d\n", argv[i], getpid());
        sleep(1);
    }

    printf("wait for 3 seconds ...\n");
    sleep(3);
    printf("Done!\n");
    return 0;
}