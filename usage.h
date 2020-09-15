#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

typedef struct
{
    int success;
    char *logfile;
    char *outfile;
    int seconds;
    char *filename;
    char *fileargs;
    int mem;
    int memkill;
} options_t;

options_t getControllerInitCommand(int argc, char *argv[])
{
    options_t op = {1, NULL, NULL, -1, NULL, NULL, -1, -1};

    // needs at least 4 arguments include the first ./controller
    if (argc < 4)
    {
        fprintf(stderr, "usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
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
            fprintf(stderr, "usage: memkill <percent>\n");
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
            if (strcmp(argv[i], "-o") == 0)
            {
                // there must be at least TWO MORE arguments following this flag
                if (argc - 1 - i >= 2)
                {
                    op.outfile = argv[i + 1];
                    i += 2;
                }
                else
                {
                    fprintf(stderr, "usage: [-o out_file] [-log log_file] [-t seconds] <file> [arg...]\n");
                    op.success = -1;
                    break;
                }
            }
            else if (strcmp(argv[i], "-log") == 0)
            {
                // there must be at least TWO MORE arguments following this flag
                if (argc - 1 - i >= 2)
                {
                    op.logfile = argv[i + 1];
                    i += 2;
                }
                else
                {
                    fprintf(stderr, "usage: [-o out_file] [-log log_file] [-t seconds] <file> [arg...]\n");
                    op.success = -1;
                    break;
                }
            }
            else if (strcmp(argv[i], "-t") == 0 && argc > i)
            {
                // there must be at least TWO MORE arguments following this flag
                if (argc - 1 - i >= 2)
                {
                    op.outfile = argv[i + 1];
                    i += 2;
                }
                else
                {
                    fprintf(stderr, "usage: [-o out_file] [-log log_file] [-t seconds] <file> [arg...]\n");
                    op.success = -1;
                    break;
                }
            }
            // get the file name to be executed
            else
            {
                op.filename = argv[i];
                char *fargs[argc - i];
                for (int j = 0; j < argc - 1; j++) {
                    fargs[j] = argv[i + j + 1];
                }
                break;
            }
        }
    }

    return op;
}