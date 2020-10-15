#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "structs.h"
#include "extensions.h"
#include "usage.h"
#include "helper.h"

#define MAX_FLOAT 10

void print_usage_error()
{
    fprintf(stderr, "usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
}

options_t getControllerInitCommand(int argc, char *argv[])
{

    options_t op = {FileExec, 1, -1, -1, 1, NULL, NULL, NULL, NULL};

    // print usage to stdout
    if (strcmp(argv[1], "--help") == 0)
    {
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
            op.type = Mem;
        }
        else
        {
            op.type = MemWithPid;
            if (is_number(argv[4])){
                op.mempid = atoi(argv[4]);
            } else {
                print_usage_error();
                op.success = -1;
                return op;
            }
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
            op.type = Memkill;
            op.memkill = (char *)exMalloc(sizeof(char) * (strlen(argv[4]) + 1));
            if (is_number(argv[4])){
                op.memkill = argv[4];
            } else {
                print_usage_error();
                op.success = -1;
                return op;
            }
        }
    }
    // other options
    else
    {

        int i = 3;
        bool o = false;
        bool l = false;
        bool t = false;
        while (i < argc)
        {
            if (strcmp(argv[i], "-o") == 0 && i % 2 != 0)
            {
                // there must be at least TWO MORE arguments following this flag
                if (argc - 1 - i >= 2 && !l && !t)
                {
                    op.outfile = (char *)exMalloc(sizeof(char) * (strlen(argv[i + 1]) + 1));
                    op.outfile = argv[i + 1];
                    i += 2;
                    o = true;
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
                if (argc - 1 - i >= 2 && !t)
                {
                    op.logfile = (char *)exMalloc(sizeof(char) * (strlen(argv[i + 1]) + 1));
                    op.logfile = argv[i + 1];
                    i += 2;
                    l = true;
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
                    op.seconds = atoi(argv[i + 1]);
                    i += 2;
                    t = true;
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
                for (int j = i; j < argc; j++)
                {
                    bufCount += strlen(argv[j]);
                    bufCount++; // for the space
                }
                op.execCommand = exMalloc(sizeof(char) * bufCount);
                op.execArgc += argc - i - 1;
                // op.sizeExecCommand = bufCount;
                // concat args and make a string of command
                strcpy(op.execCommand, argv[i]);
                int current = 0;
                for (int j = i + 1; j < argc; j++)
                {
                    strcat(op.execCommand, " ");
                    strcat(op.execCommand, argv[j]);
                }
                break;
            }
        } // while loop
        op.type = FileExec;
    } // else

    return op;
}