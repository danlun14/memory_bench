#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>

#define L1_SIZE 192 * 1024
#define L2_SIZE 3 * 1024 * 1024
#define L3_SIZE 32 * 1024 * 1024

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

void print_out_res_file(out_res node)
{
    FILE *out_file = fopen("result.csv", "a");
    fprintf(out_file, "[MemoryType: %s; ", node.MemoryType);
    fprintf(out_file, "BlockSize: %ld; ", node.BlockSize);
    fprintf(out_file, "ElmentType: %s; ", node.ElementType);
    fprintf(out_file, "BufferSize: %d; ", node.BufferSize);
    fprintf(out_file, "LaunchNum: %d; ", node.LaunchNum);
    fprintf(out_file, "Timer: %s; ", node.Timer);

    fprintf(out_file, "WriteTime: %.5lf; ", node.WriteTime);
    fprintf(out_file, "AverageWriteTime: %.5lf; ", node.AverageWriteTime);
    fprintf(out_file, "WriteBandwidth: %.5lf; ", node.WriteBandwidth);
    fprintf(out_file, "AbsErrorWrite: %.5lf; ", node.AbsErrorWrite);
    fprintf(out_file, "RelErrorWrite: %.5lf %%; ", node.RelErrorWrite);

    fprintf(out_file, "ReadTime: %.5lf; ", node.ReadTime);
    fprintf(out_file, "AverageReadTime: %.5lf; ", node.AverageReadTime);
    fprintf(out_file, "ReadBandwidth: %.5lf; ", node.ReadBandwidth);
    fprintf(out_file, "AbsErrorRead: %.5lf; ", node.AbsErrorRead);
    fprintf(out_file, "RelErrorRead: %.5lf %%;]\n", node.RelErrorRead);
    fclose(out_file);
}

//allocate and init with eng alphabet
char *memory_init(long count)
{
    char *charptr = malloc(count);
    if (charptr == NULL)
    {
        printf("memory allocate error\n");
        exit(1);
    }
    for (int i = 0; i < count; i++)
    {
        *(charptr + i) = i % 27 + 'a';
    }
    return charptr;
}

void *xmalloc(int count)
{
    void *ptr = malloc(count);
    if (ptr == NULL)
    {
        printf("memory allocate error\n");
        exit(1);
    }
    else
    {
        return ptr;
    }
}

void ram_write1(long *vector, long count)
{
    for (int i = 0; i < count; i += 1)
    {
        *(vector + i) = i;
    }
}
void ram_write(long *vector, long count)
{
    for (int i = 0; i < count; i += 4)
    {
        *(vector + i) = i;
        *(vector + i + 1) = i + 1;
        *(vector + i + 2) = i + 2;
        *(vector + i + 3) = i + 3;
    }
}

int ram_read(long *vector, long count)
{
    long v1 = 0, v2 = 0, v3 = 0, v4 = 0, v5 = 0, v6 = 0;
    for (int i = 0; i < count; i += 4)
    {
        v1 = *(vector + i);
        v2 = *(vector + i + 1);
        v3 = *(vector + i + 2);
        v4 = *(vector + i + 3);
        //v5 = *(vector + i + 4);
        //v6 = *(vector + i + 5);
    }
    return v1 + v2 + v3 + v4;
}

void default_memory_test()
{
    parsed_args pargs;

    pargs.count = 50;
    pargs.block_size = 4;
    pargs.block_size_measure = MB;
    long bytes = 0;
    bytes = pargs.block_size * pow(1024, pargs.block_size_measure);
    char *pathname = "test_default_memory123adad13HTHTEFG.txt";
    //read(STDIN_FILENO, pathname, 128);
    //ссд
    FILE *result_blocks_wr = fopen("plot_blocksr.txt", "w");
    FILE *result_blocks_rd = fopen("plot_blocksw.txt", "w");
    for (pargs.block_size; pargs.block_size <= 80; pargs.block_size += 4)
    {
        out_res smem_res[pargs.count];
        bytes = pargs.block_size * pow(1024, pargs.block_size_measure);

        char *char_array = memory_init(bytes);
        //out_res smem_res[pargs.count];
        double t_run = 0;
        double t_avr_wr = 0;
        double t_avr_rd = 0;

        for (int i = 0; i < pargs.count; i++)
        {
            smem_res[i].MemoryType = smem_res[0].MemoryType;
            smem_res[i].BlockSize = bytes;
            smem_res[i].ElementType = "CHAR";
            smem_res[i].BufferSize = 0;
            smem_res[i].LaunchNum = i + 1;
            smem_res[i].Timer = "hpctimer_getwtime()";
            FILE *opfile;
            if ((opfile = fopen(pathname, "w")) == NULL)
            {
                printf("Cannot open file.\n");
                exit(1);
            }

            setvbuf(opfile, char_array, _IOFBF, BUFSIZ);
            t_run = 0 - hpctimer_getwtime();
            int test = fwrite(char_array, sizeof(char), bytes, opfile);
            if (test != bytes)
            {

                getchar();
                printf("Cannot write %ld.\n", bytes);
                //exit(1);
            }
            else
            {
                //  printf("write.\n");
            }
            t_run += hpctimer_getwtime();
            t_avr_wr += t_run;
            smem_res[i].WriteTime = t_run;
            //printf("write - %lf\n", t_run);
            //fflush(stdout);

            fclose(opfile);

            if ((opfile = fopen(pathname, "r")) == NULL)
            {
                printf("Cannot open file.\n");
                exit(1);
            }
            char *_char_array = xmalloc(bytes);
            setvbuf(opfile, char_array, _IOFBF, BUFSIZ);
            t_run = 0 - hpctimer_getwtime();
            test = fread(_char_array, sizeof(char), bytes, opfile);
            if (test != bytes)
            {
                printf("Cannot read.\n");
                //exit(1);
            }
            else
            {
                //printf("read\n");
            }
            t_run += hpctimer_getwtime();
            t_avr_rd += t_run;
            smem_res[i].ReadTime = t_run;
            //printf("read - %lf\n", t_run);
            fflush(stdout);
            free(_char_array);
            fclose(opfile);

            unlink(pathname);
        }

        for (int i = 0; i < pargs.count; i++)
        {
            smem_res[i].AverageWriteTime = t_avr_wr / pargs.count;
            smem_res[i].WriteBandwidth = bytes / smem_res[0].AverageWriteTime / (1024 * 1024);
            smem_res[i].AbsErrorWrite = fabs(smem_res[i].AverageWriteTime - smem_res[i].WriteTime);
            smem_res[i].RelErrorWrite = smem_res[i].AbsErrorWrite / smem_res[i].AverageWriteTime;

            smem_res[i].AverageReadTime = t_avr_rd / pargs.count;
            smem_res[i].ReadBandwidth = bytes / smem_res[0].AverageReadTime / (1024 * 1024);
            smem_res[i].AbsErrorRead = fabs(smem_res[i].AverageReadTime - smem_res[i].ReadTime);
            smem_res[i].RelErrorRead = smem_res[i].AbsErrorRead / smem_res[i].AverageReadTime;
            //print_out_res(smem_res[i]);
            print_out_res_file(smem_res[i]);
        }
        fprintf(result_blocks_wr, "%d %.0lf\n", pargs.block_size, smem_res[0].WriteBandwidth);
        fprintf(result_blocks_rd, "%d %.0lf\n", pargs.block_size, smem_res[0].ReadBandwidth);
    }
    fclose(result_blocks_wr);
    fclose(result_blocks_rd);
    /*
    bytes = L1;
    out_res rammem_res[pargs.count];
    double t_run = 0;
    double t_avr_wr = 0;
    double t_avr_rd = 0;
    for (int i = 0; i < pargs.count; i++)
    {
        rammem_res[i].MemoryType = "RAM";
        rammem_res[i].BlockSize = bytes;
        rammem_res[i].ElementType = "LONG";
        rammem_res[i].BufferSize = 0;
        rammem_res[i].LaunchNum = i + 1;
        rammem_res[i].Timer = "hpctimer_getwtime()";

        long *vector = xmalloc(bytes);
        t_run = 0 - hpctimer_getwtime();
        fflush(stdout);
        ram_write(vector, bytes / sizeof(long));
        t_run += hpctimer_getwtime();
        t_avr_wr += t_run;
        free(vector);
        fflush(stdout);
        printf("write - %lf\n", t_run);
        fflush(stdout);

        rammem_res[i].WriteTime = t_run;

        vector = xmalloc(bytes);
        t_run = 0 - hpctimer_getwtime();
        ram_read(vector, bytes / sizeof(long));
        t_run += hpctimer_getwtime();
        t_avr_rd += t_run;
        free(vector);
        printf("read - %lf\n", t_run);
        fflush(stdout);

        rammem_res[i].ReadTime = t_run;
    }

    for (int i = 0; i < pargs.count; i++)
    {
        rammem_res[i].AverageWriteTime = t_avr_wr / pargs.count;
        rammem_res[i].WriteBandwidth = bytes / rammem_res[0].AverageWriteTime / (1024 * 1024);
        rammem_res[i].AbsErrorWrite = fabs(rammem_res[i].AverageWriteTime - rammem_res[i].WriteTime);
        rammem_res[i].RelErrorWrite = rammem_res[i].AbsErrorWrite / rammem_res[i].AverageWriteTime;

        rammem_res[i].AverageReadTime = t_avr_rd / pargs.count;
        rammem_res[i].ReadBandwidth = bytes / rammem_res[0].AverageReadTime / (1024 * 1024);
        rammem_res[i].AbsErrorRead = fabs(rammem_res[i].AverageReadTime - rammem_res[i].ReadTime);
        rammem_res[i].RelErrorRead = rammem_res[i].AbsErrorRead / rammem_res[i].AverageReadTime;
        print_out_res(rammem_res[i]);
        print_out_res_file(rammem_res[i]);
    }
    
    */
}

void ram_memory_test(parsed_args pargs)
{
    long bytes = pargs.block_size * pow(1024, pargs.block_size_measure);
    out_res rammem_res[pargs.count];
    double t_run = 0;
    double t_avr_wr = 0;
    double t_avr_rd = 0;
    for (int i = 0; i < pargs.count; i++)
    {
        rammem_res[i].MemoryType = "RAM";
        rammem_res[i].BlockSize = bytes;
        rammem_res[i].ElementType = "LONG";
        rammem_res[i].BufferSize = 0;
        rammem_res[i].LaunchNum = i + 1;
        rammem_res[i].Timer = "hpctimer_getwtime()";

        long *vector = xmalloc(bytes);
        t_run = 0 - hpctimer_getwtime();
        fflush(stdout);
        ram_write(vector, bytes / sizeof(long));
        t_run += hpctimer_getwtime();
        t_avr_wr += t_run;
        free(vector);
        fflush(stdout);
        printf("write - %lf\n", t_run);
        fflush(stdout);

        rammem_res[i].WriteTime = t_run;

        vector = xmalloc(bytes);
        t_run = 0 - hpctimer_getwtime();
        ram_read(vector, bytes / sizeof(long));
        t_run += hpctimer_getwtime();
        t_avr_rd += t_run;
        free(vector);
        printf("read - %lf\n", t_run);
        fflush(stdout);

        rammem_res[i].ReadTime = t_run;
    }

    for (int i = 0; i < pargs.count; i++)
    {
        rammem_res[i].AverageWriteTime = t_avr_wr / pargs.count;
        rammem_res[i].WriteBandwidth = bytes / rammem_res[0].AverageWriteTime / (1024 * 1024);
        rammem_res[i].AbsErrorWrite = fabs(rammem_res[i].AverageWriteTime - rammem_res[i].WriteTime);
        rammem_res[i].RelErrorWrite = rammem_res[i].AbsErrorWrite / rammem_res[i].AverageWriteTime;

        rammem_res[i].AverageReadTime = t_avr_rd / pargs.count;
        rammem_res[i].ReadBandwidth = bytes / rammem_res[0].AverageReadTime / (1024 * 1024);
        rammem_res[i].AbsErrorRead = fabs(rammem_res[i].AverageReadTime - rammem_res[i].ReadTime);
        rammem_res[i].RelErrorRead = rammem_res[i].AbsErrorRead / rammem_res[i].AverageReadTime;
        print_out_res(rammem_res[i]);
        print_out_res_file(rammem_res[i]);
    }
}

void swap_memory_test(parsed_args pargs)
{
    system("./free_ram.sh > extalsskgkb14787885213.txt");
    int fd = open("extalsskgkb14787885213.txt", O_RDONLY);
    char buffer;
    long kbytes_to_full_ram = 0;
    while (read(fd, &buffer, 1))
    {
        kbytes_to_full_ram *= 10;
        kbytes_to_full_ram += buffer - '0';
    }
    printf("%ld\n", kbytes_to_full_ram);
    long *to_delete = xmalloc((long)kbytes_to_full_ram * 1024 + (long)sizeof(long) * 1000 * 1000 * 1000 * 3);
    printf("lol\n");
    ram_write1(to_delete, (long)kbytes_to_full_ram * 1024 / sizeof(long) + (long)1000 * 1000 * 1000 * 3);
    getchar();
    free(to_delete);
    unlink("extalsskgkb14787885213.txt");
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
    out_res smem_res[pargs.count];
    //
    if ((fd = open(pathname, O_CREAT | O_RDONLY | O_EXCL | O_APPEND, S_IRWXU)) == -1)
    {
        printf("Cannot create file.\n");
        exit(1);
    }
    close(fd);
    double t_run = 0;
    double t_avr_wr = 0;
    double t_avr_rd = 0;
    int operation_result = 0;
    char *char_array = memory_init(bytes);
    if (pargs.mem_type == 3)
    {
        smem_res[0].MemoryType = "FLASH";
    }
    if (pargs.mem_type == 2)
    {
        smem_res[0].MemoryType = "SSD|HDD";
    }
    for (int i = 0; i < pargs.count; i++)
    {
        smem_res[i].MemoryType = smem_res[0].MemoryType;
        smem_res[i].BlockSize = bytes;
        smem_res[i].ElementType = "CHAR";
        smem_res[i].BufferSize = 0;
        smem_res[i].LaunchNum = i + 1;
        smem_res[i].Timer = "hpctimer_getwtime()";

        if ((fd = open(pathname, O_WRONLY | O_APPEND, S_IRWXU)) == -1)
        {
            printf("Cannot open file.\n");
            exit(1);
        }
        t_run = 0 - hpctimer_getwtime();
        operation_result = write(fd, char_array, bytes);
        //fsync(fd);
        if (operation_result != bytes)
        {
            printf("Cannot write.\n");
            //exit(1);
        }
        t_run += hpctimer_getwtime();
        t_avr_wr += t_run;
        printf("write - %lf\n", t_run);
        fflush(stdout);

        smem_res[i].WriteTime = t_run;

        char *_char_ptr = xmalloc(bytes);
        t_run = 0 - hpctimer_getwtime();

        close(fd);
        if ((fd = open(pathname, O_RDONLY | O_APPEND, S_IRWXU)) == -1)
        {
            printf("Cannot open file.\n");
            exit(1);
        }
        operation_result = read(fd, _char_ptr, bytes);
        //fsync(fd);
        if (operation_result != bytes)
        {
            printf("read error.\n");
            //exit(1);
        }
        t_run += hpctimer_getwtime();
        smem_res[i].ReadTime = t_run;
        t_avr_rd += t_run;
        printf("read - %lf\n", t_run);
        free(_char_ptr);
        fflush(stdout);
        close(fd);
    }

    for (int i = 0; i < pargs.count; i++)
    {
        smem_res[i].AverageWriteTime = t_avr_wr / pargs.count;
        smem_res[i].WriteBandwidth = bytes / smem_res[0].AverageWriteTime / (1024 * 1024);
        smem_res[i].AbsErrorWrite = fabs(smem_res[i].AverageWriteTime - smem_res[i].WriteTime);
        smem_res[i].RelErrorWrite = smem_res[i].AbsErrorWrite / smem_res[i].AverageWriteTime;

        smem_res[i].AverageReadTime = t_avr_rd / pargs.count;
        smem_res[i].ReadBandwidth = bytes / smem_res[0].AverageReadTime / (1024 * 1024);
        smem_res[i].AbsErrorRead = fabs(smem_res[i].AverageReadTime - smem_res[i].ReadTime);
        smem_res[i].RelErrorRead = smem_res[i].AbsErrorRead / smem_res[i].AverageReadTime;
        //print_out_res(smem_res[i]);
        print_out_res_file(smem_res[i]);
    }
    //printf("%lfMB\n", bytes / smem_res[0].AverageWriteTime / (1024 * 1024));
    //printf("Press enter to continue...");
    //getchar();
    unlink(pathname);
}

void test_memory(parsed_args pargs)
{
    if (pargs.mem_type == DEFAULT_MEM_TEST)
    {
        default_memory_test();
    }
    else if (pargs.mem_type == RAM)
    {
        ram_memory_test(pargs);
    }
    else if (pargs.mem_type == SWAP)
    {
        printf("Non supported!");
        //swap_memory_test(pargs);
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