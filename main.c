#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "hpctimer.h"
#include "CLI.h"

typedef struct
{
    char *PModel;
    char *Task;
    char *OpType;
    long InsCount;
    char *OptFlag;
    char *Timer;
    double Time;
    int LNum;
    double AvTime;
    double AbsError;
    double RelError;
    long TaskPerf;
} out_res; //11(5 st 6 on)

void print_out_res(out_res node)
{
    printf("PModel: %s; ", node.PModel);
    printf("Task: %s; ", node.Task);
    printf("OpType: %s; ", node.OpType);
    printf("InstCount: %ld; ", node.InsCount);
    printf("OptFlag: %s; ", node.OptFlag);
    printf("Time: %.4lf; ", node.Time);
    printf("LNum: %d; ", node.LNum);
    printf("AvTime: %.4lf; ", node.AvTime);
    printf("AbsError: %.4lf; ", node.AbsError);
    printf("RelError: %.4lf %; ", node.RelError);
    printf("TaskPerf: %ld;\n", node.TaskPerf);
}

void print_out_res_file(out_res node, FILE *out_file)
{
    fprintf(out_file, "PModel: %s; ", node.PModel);
    fprintf(out_file, "Task: %s; ", node.Task);
    fprintf(out_file, "OpType: %s; ", node.OpType);
    fprintf(out_file, "InstCount: %ld; ", node.InsCount);
    fprintf(out_file, "OptFlag: %s; ", node.OptFlag);
    fprintf(out_file, "Time: %.4lf; ", node.Time);
    fprintf(out_file, "LNum: %d; ", node.LNum);
    fprintf(out_file, "AvTime: %.4lf; ", node.AvTime);
    fprintf(out_file, "AbsError: %.4lf; ", node.AbsError);
    fprintf(out_file, "RelError: %.4lf %; ", node.RelError);
    fprintf(out_file, "TaskPerf: %ld;\n", node.TaskPerf);
}

char output_buf[256];

int main(int argc, char *argv[])
{
    parsed_args pargs = parse_arguments(argc, argv);
    printf("Memory type - %s\n", pargs.mem_type);
    printf("block size - %d\n", pargs.block_size);
    printf("block size measure - %s\n", pargs.block_size_measure);
    printf("Count - %d\n", pargs.count);
}