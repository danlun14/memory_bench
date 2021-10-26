#include "CLI.h"

const char *arguments[ARG_MAX_LENGHT];

int strcmp_m(const char *arg, const char *ava_arg)
{
    int id = 0;
    while (*(arg + id) != '\0')
    {
        if (*(arg + id) != *(ava_arg + id))
        {
            //printf("id - %d\n", id);
            return -1;
        }
        id++;
    }
    return id;
}

void arg_init()
{
    arguments[0] = "--memory-type";
    arguments[1] = "--block-size";
    arguments[2] = "--reps";
}

int len_num(float num)
{
    int result = 0;
    while (num >= 1)
    {
        num /= 10;
        result++;
    }
    return result;
}

parsed_args parse_arguments(int argc, char *argv[])
{
    parsed_args pargs;

    arg_init();

    pargs.mem_type = "RAM";
    pargs.block_size_measure = NULL;
    pargs.block_size = 1024;
    pargs.count = 10;

    int parsed_number = 0;
    int status = 0;
    for (int i = 0; i < argc; i++)
    {
        //printf("%s\n", argv[i]);
        for (int j = 0; j < 3; j++)
        {
            status = strcmp_m(arguments[j], argv[i]);
            // printf("  s-%d\n", status);
            if (status != -1)
            {
                parsed_number = atoi(argv[i] + status);
                // printf("  %d\n", parsed_arg);
                switch (j)
                {
                case 0:
                    if (strcmp_m(argv[i] + status, "RAM") != -1)
                    {
                        pargs.mem_type = "RAM";
                    }
                    else if (strcmp_m(argv[i] + status, "HDD") != -1)
                    {
                        pargs.mem_type = "HDD";
                    }
                    if (strcmp_m(argv[i] + status, "SSD") != -1)
                    {
                        pargs.mem_type = "SSD";
                    }
                    if (strcmp_m(argv[i] + status, "flash") != -1)
                    {
                        pargs.mem_type = "flash";
                    }

                    break;
                case 1:

                    pargs.block_size = parsed_number;
                    if (strcmp_m(argv[i] + status + len_num((float)parsed_number), "Kb") > 0)
                    {
                        pargs.block_size_measure = "Kb";
                    }
                    else if (strcmp_m(argv[i] + status + len_num((float)parsed_number), "Mb") > 0)
                    {
                        pargs.block_size_measure = "Mb";
                    }
                    break;
                case 2:
                    pargs.count = parsed_number;
                    break;
                }
            }
        }
        //printf(" %d\n", j);

        //printf("%d\n", i);
    }
    return pargs;
}