#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
// #include "extensions.h"


// pid_t execute_in_process(char *args) {

//     pid_t pid = fork();
//     if (pid < 0) {
//         exPerror("fork");
//     } else if (pid == 0) {
//         execv(args[0], &args[0]);
//     }
    
// }