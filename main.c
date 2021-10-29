#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>

#include "hpctimer.h"
#include "CLI.h"

typedef struct
{
    char *MemoryType;
    long BlockSize;
    char *ElementType;
    int BufferSize;
    int LaunchNum;
    char *Timer;

    double WriteTime;
    double AverageWriteTime;
    double WriteBandwidth;
    double AbsErrorWrite;
    double RelErrorWrite;

    double ReadTime;
    double AverageReadTime;
    double ReadBandwidth;
    double AbsErrorRead;
    double RelErrorRead;
} out_res; //11(5 st 6 on)

void print_out_res(out_res node)
{
    printf("MemoryType: %s; ", node.MemoryType);
    printf("BlockSize: %ld; ", node.BlockSize);
    printf("ElmentType: %s; ", node.ElementType);
    printf("BufferSize: %d; ", node.BufferSize);
    printf("LaunchNum: %d; ", node.LaunchNum);
    printf("Timer: %s; ", node.Timer);

    printf("WriteTime: %lf; ", node.WriteTime);
    printf("AverageWriteTime: %.4lf; ", node.AverageWriteTime);
    printf("WriteBandwidth: %.4lf; ", node.WriteBandwidth);
    printf("AbsErrorWrite: %.4lf %%; ", node.AbsErrorWrite);
    printf("RelErrorWrite: %lf;", node.RelErrorWrite);

    printf("ReadTime: %lf; ", node.ReadTime);
    printf("AverageReadTime: %.4lf; ", node.AverageReadTime);
    printf("ReadBandwidth: %.4lf; ", node.ReadBandwidth);
    printf("AbsErrorRead: %.4lf %%; ", node.AbsErrorRead);
    printf("RelErrorRead: %lf;\n", node.RelErrorRead);
}

void print_out_res_file(out_res node, FILE *out_file)
{
    fprintf(out_file, "[MemoryType: %s; ", node.MemoryType);
    fprintf(out_file, "BlockSize: %ld; ", node.BlockSize);
    fprintf(out_file, "ElmentType: %s; ", node.ElementType);
    fprintf(out_file, "BufferSize: %d; ", node.BufferSize);
    fprintf(out_file, "LaunchNum: %d; ", node.LaunchNum);
    fprintf(out_file, "Timer: %s; ", node.Timer);

    fprintf(out_file, "WriteTime: %lf; ", node.WriteTime);
    fprintf(out_file, "AverageWriteTime: %.4lf; ", node.AverageWriteTime);
    fprintf(out_file, "WriteBandwidth: %.4lf; ", node.WriteBandwidth);
    fprintf(out_file, "AbsErrorWrite: %.4lf %%; ", node.AbsErrorWrite);
    fprintf(out_file, "RelErrorWrite: %lf;", node.RelErrorWrite);

    fprintf(out_file, "ReadTime: %lf; ", node.ReadTime);
    fprintf(out_file, "AverageReadTime: %.4lf; ", node.AverageReadTime);
    fprintf(out_file, "ReadBandwidth: %.4lf; ", node.ReadBandwidth);
    fprintf(out_file, "AbsErrorRead: %.4lf %%; ", node.AbsErrorRead);
    fprintf(out_file, "RelErrorRead: %lf;]\n", node.RelErrorRead);
}

//allocate and init with eng alphabet
char *memory_init(long count)
{
    char *charptr = malloc(count);
    for (int i = 0; i < count; i++)
    {
        *(charptr + i) = i % 27 + 'a';
    }
    return (charptr == NULL) ? NULL : charptr;
}

void default_memory_test(parsed_args pargs)
{
}

void ram_memory_test(parsed_args pargs)
{
    long bytes = pargs.block_size * (1024 ^ pargs.block_size_measure);
}

void memory_test(parsed_args pargs)
{
    long bytes = pargs.block_size * pow(1024, pargs.block_size_measure);

    printf("%ld\n", bytes);
    int fd;
    char pathname[128];
    printf("Enter filename, on testing disk: ");
    fflush(stdout);
    read(STDIN_FILENO, pathname, 128);

    /*
    if ((fd = open(pathname, O_CREAT | O_WRONLY | O_EXCL | O_APPEND, S_IRWXU)) == -1)
    {
        printf("Cannot open file.\n");
        exit(1);
    }
    //out_res smem_res[pargs.count];
    double t_run = 0;
    double t_avr = 0;
    char *char_array = memory_init(bytes);
    for (int i = 0; i < pargs.count; i++)
    {
        t_run = 0 - hpctimer_getwtime();
        int test = write(fd, char_array, bytes);
        fsync(fd);
        if (test != bytes)
        {
            printf("Cannot write.\n");
            //exit(1);
        }
        t_run += hpctimer_getwtime();
        t_avr += t_run;
        printf("%lf\n", t_run);
        fflush(stdout);
    }
    close(fd);
    */

    FILE *opfile;
    if ((opfile = fopen(pathname, "a")) == NULL)
    {
        printf("Cannot open file.\n");
        exit(1);
    }
    //out_res smem_res[pargs.count];
    double t_run = 0;
    double t_avr = 0;
    char *char_array = memory_init(bytes);
    for (int i = 0; i < pargs.count; i++)
    {
        t_run = 0 - hpctimer_getwtime();
        int test = fwrite(char_array, sizeof(char), bytes, opfile);
        if (test != bytes)
        {
            printf("Cannot write.\n");
            //exit(1);
        }
        t_run += hpctimer_getwtime();
        t_avr += t_run;
        printf("%lf\n", t_run);
        fflush(stdout);
    }
    fclose(opfile);
    printf("Press enter to continue...");
    getchar();
    //unlink(pathname);
}

void test_memory(parsed_args pargs)
{
    if (pargs.mem_type == DEFAULT_MEM_TEST)
    {
        default_memory_test(pargs);
    }
    else if (pargs.mem_type == RAM)
    {
        ram_memory_test(pargs);
    }
    else
    {
        memory_test(pargs);
    }
}

int main(int argc, char *argv[])
{
    parsed_args pargs = parse_arguments(argc, argv);
    printf("Memory type - %d\n", pargs.mem_type);
    printf("block size - %d\n", pargs.block_size);
    printf("block size measure - %d\n", pargs.block_size_measure);
    printf("Count - %d\n", pargs.count);
    printf("---------------------------------------\n");
    test_memory(pargs);
}