
// You may only use fgets() to pull input from stdin
// You may use any print function to stdout to print 
// out chat messages
#include <stdio.h>

// You may use memory allocators and helper functions 
// (e.g., rand()).  You may not use system().
#include <stdlib.h>

#include <inttypes.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#ifndef UTIL_H_
#define UTIL_H_

#define ADDR_PTR uint64_t 
#define CYCLES uint32_t
#define BUFF_SIZE (1<<(21))
#define TransmittingSignal 11
#define StartIndex 0
#define StopIndex 8
#define OffsetA 16
#define OffsetB 27
#define OffsetC 38
#define OffsetD 55
CYCLES measure_one_block_access_time(ADDR_PTR addr);

// You Should Not Use clflush in your final submission
// It is only used for debug
void clflush(ADDR_PTR addr);

char *string_to_binary(char *s);
char *binary_to_string(char *data);

int string_to_int(char* s);
void wait(int time);
#endif
