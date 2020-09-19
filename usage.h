#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "structs.h"

void print_usage_error() {
    fprintf(stderr, "usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
}

/*
 * Check the input arguments and retrieves each of arguments such as IP address, file names etc.
 * Print usage error if user inputs undefined command and returns struct options_t that stores the user input
 * options_t defined in structs.h 
 */
options_t getControllerInitCommand(int argc, char *argv[])
{

    options_t op = {1, -1, -1, -1, 1, NULL, NULL, NULL};

    if (strcmp(argv[1], "--help") == 0) {
        printf("usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
        op.success = -1;
        return op;
    }

    // needs at least 4 arguments include the first ./controller
    if (argc < 4)
    {
        print_usage_error();
        op.success = -1;
        return op;
    }

    // mem option
    if (strcmp(argv[3], "mem") == 0)
    {
        if (argc < 5)
        {
            printf("mem option without pid\n");
        }
        else
        {
            printf("mem option with [pid] = %d\n", atoi(argv[4]));
        }
    }
    // memkill option
    else if (strcmp(argv[3], "memkill") == 0)
    {
        if (argc < 5)
        {
            print_usage_error();
            op.success = -1;
        }
        else
        {
            printf("memkill option with <percent> = %d\n", atoi(argv[4]));
        }
    }
    // other options
    else
    {
        int i = 3;
        while (i < argc)
        {
            if (strcmp(argv[i], "-o") == 0 && i % 2 != 0)
            {
                // there must be at least TWO MORE arguments following this flag
                if (argc - 1 - i >= 2)
                {
                    op.outfile = (char *)malloc(sizeof(char) * (strlen(argv[i +1]) + 1));
                    op.outfile = argv[i + 1];
                    i += 2;
                }
                else
                {
                    print_usage_error();
                    op.success = -1;
                    break;
                }
            }
            else if (strcmp(argv[i], "-log") == 0 && i % 2 != 0)
            {
                // there must be at least TWO MORE arguments following this flag
                if (argc - 1 - i >= 2)
                {
                    op.logfile = (char *)malloc(sizeof(char) * (strlen(argv[i +1]) + 1));
                    op.logfile = argv[i + 1];
                    i += 2;
                }
                else
                {
                    print_usage_error();
                    op.success = -1;
                    break;
                }
            }
            else if (strcmp(argv[i], "-t") == 0 && i % 2 != 0)
            {
                // there must be at least TWO MORE arguments following this flag
                if (argc - 1 - i >= 2)
                {
                    op.seconds = atoi(argv[i+1]);
                    i += 2;
                }
                else
                {
                    print_usage_error();
                    op.success = -1;
                    break;
                }
            }
            // get the file name to be executed
            else
            { 
                // pre (i == the index of filename)
                // create buf for the exec command 
                int bufCount = 0;
                for (int j=i; j < argc; j++) {
                    bufCount += strlen(argv[j]);
                    bufCount++; // for the space
                }
                op.execCommand = malloc(sizeof(char) * bufCount);
                op.execArgc += argc - i - 1;
                // op.sizeExecCommand = bufCount;
                // concat args and make a string of command
                strcpy(op.execCommand, argv[i]);
                int current = 0;
                for (int j=i+1; j < argc; j++) {
                    strcat(op.execCommand, " ");
                    strcat(op.execCommand, argv[j]);
                }
                break;
            }
        } // while loop
    } // else

    return op;
}