#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#ifndef CLI_H
#define CLI_H

#define MB 2
#define KB 1
#define BYTES 0
#define DEFAULT_BLOCK_SIZE_MEASURE -1
#define DEFAULT_BLOCK_SIZE -1

#define DEFAULT_MEM_TEST 0
#define FLASH 1
#define SSDnHDD 2
#define RAM 3
#define SWAP 4

#define ARG_MAX_LENGHT 32

typedef struct
{
    int mem_type;
    int block_size;
    int block_size_measure;
    int count;
} parsed_args;

int strcmp_m(const char *arg, const char *ava_arg);
parsed_args parse_arguments(int argc, char *argv[]);
void arg_init();

#endif