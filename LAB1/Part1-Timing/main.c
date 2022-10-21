#include "utility.h"
#include "time.h"
// TODO: Uncomment the following lines and fill in the correct size
//#define L1_SIZE [TODO]
//#define L2_SIZE [TODO]
//#define L3_SIZE [TODO]


// https://www.geeksforgeeks.org/shuffle-a-given-array-using-fisher-yates-shuffle-algorithm/
void swap (uint64_t *a, uint64_t *b)
{
    uint64_t temp = *a;
    *a = *b;
    *b = temp;
}
void randomize ( uint64_t arr[], uint64_t n )
{
    srand ( time(NULL) );
    for (uint64_t i = n-1; i > 0; i--)
    {
        uint64_t j = rand() % (i+1);
        swap(&arr[i], &arr[j]);
    }
}
// void swap (uint64_t* a, uint64_t* b)
// {
//     uint64_t temp = *a;
//     *a = *b;
//     *b = temp;
// }
// void randomize ( uint64_t** arr, int n )
// {
//     srand ( time(NULL) );
//     for (int i = n-1; i > 0; i--)
//     {
//         int j = rand() % (i+1);
//         swap(&arr[i], &arr[j]);
//     }
// }

void wait(int time){
  uint64_t foo = rdtscp64();
  while (rdtscp64()-foo<time);
}


int main (int ac, char **av) {

    // create 4 arrays to store the latency numbers
    // the arrays are initialized to 0
    uint64_t dram_latency[SAMPLES] = {0};
    uint64_t l1_latency[SAMPLES] = {0};
    uint64_t l2_latency[SAMPLES] = {0};
    uint64_t l3_latency[SAMPLES] = {0};

    // A temporary variable we can use to load addresses
    // The volatile keyword tells the compiler to not put this variable into a
    // register- it should always try to load from memory/ cache.
    volatile char tmp;

    // Allocate a buffer of 64 Bytes
    // the size of an unsigned integer (uint64_t) is 8 Bytes
    // Therefore, we request 8 * 8 Bytes
    uint64_t *target_buffer = (uint64_t *)malloc(8*sizeof(uint64_t));

    if (NULL == target_buffer) {
        perror("Unable to malloc");
        return EXIT_FAILURE;
    }

    // [1.4] TODO: Uncomment the following line to allocate a buffer of a size
    // of your chosing. This will help you measure the latencies at L2 and L3.
    // 64 bytes * 8 way
    // uint64_t *eviction_buffer = (uint64_t)malloc(sizeof(uint64_t) * 8 * 8);

    // wait(100000); //Warmup?


    

    // ======
    // [1.4] TODO: Measure L3 Latency, store results in l3_latency array
    // ======
    size_t bytesToFill =(size_t ) 32768 + 262144 + (1<<15);
    size_t amtOfThings = bytesToFill / sizeof(uint64_t*);
    uint64_t** test = malloc(bytesToFill);
    uint64_t* foo = (uint64_t *) malloc(1*sizeof(uint64_t));


    uint64_t* rdmList = calloc(amtOfThings,sizeof(uint64_t));
    for (size_t i = 0; i < amtOfThings; i++) rdmList[i] = i;
    randomize(rdmList, amtOfThings);
    
    for (size_t i = 0; i < amtOfThings-1; i++)
        test[rdmList[i]] =  (uint64_t*) test + rdmList[i+1];
    
    test[rdmList[amtOfThings-1]] = (uint64_t*) test + rdmList[0];
    uint64_t** pnt = test;
    for (size_t a = 0; a < SAMPLES; a++)//Multiple times to warm up CPU
    for (size_t i = 0; i < SAMPLES; i++)
    {
        tmp = *foo;
        *foo = i;
        
        for (size_t j = 0; j < amtOfThings * 10; j++)
            pnt = *pnt;
        
        l3_latency[i] = measure_one_block_access_time((uint64_t)foo);
        
    }
    free(foo);
    free(test);
    free(rdmList);

    

     //===
     //L1
     //===
        // Example: Measure L1 access latency, store results in l1_latency array
    for (int i=0; i<SAMPLES; i++){
        // Step 1: bring the target cache line into L1 by simply accessing the line
        tmp = target_buffer[0];

        // Step 2: measure the access latency
        l1_latency[i] = measure_one_block_access_time((uint64_t)target_buffer);
    }


    //==
    //L2
    //==

    bytesToFill = (size_t )32768 + (1<<15);
    amtOfThings = bytesToFill / sizeof(uint64_t**);
    test = (uint64_t**) malloc(bytesToFill);
    foo = (uint64_t *) malloc(1*sizeof(uint64_t));
    *foo = 556688;
    rdmList = calloc(amtOfThings,sizeof(uint64_t));
    for (size_t i = 0; i < amtOfThings; i++) rdmList[i] = i;
    randomize(rdmList, amtOfThings);
    
    for (size_t i = 0; i < amtOfThings-1; i++)
        test[rdmList[i]] =  (uint64_t*) test + rdmList[i+1];
    
    test[rdmList[amtOfThings-1]] = (uint64_t*) test + rdmList[0];
    
    pnt =  test;
    for (size_t a = 0; a < SAMPLES; a++) //Multiple times to warm up CPU
    for (size_t i = 0; i < SAMPLES; i++)
    {
        tmp = *foo;
        *foo = i;
        //Fill up L1
        for (size_t j = 0; j < amtOfThings*10; j++)
            pnt = *pnt;
        
        l2_latency[i] = measure_one_block_access_time((uint64_t)foo);
        
    }
    free(foo);
    free(test);
    free(rdmList);
 
    // ======
    // [1.4] TODO: Measure DRAM Latency, store results in dram_latency array
    // ======
    //
    uint64_t *dramTest = (uint64_t *)malloc(1*sizeof(uint64_t));
    *dramTest = 1234567890;
    for (size_t i = 0; i < SAMPLES; i++)
    {
        //Clear from cache
        clflush(dramTest);
        //Read from DRAM, its not in the cache
        dram_latency[i] = measure_one_block_access_time((uint64_t)dramTest);
    }
    free(dramTest);


    // Print the results to the screen
    // [1.5] Change print_results to print_results_for_python so that your code will work
    // with the python plotter software
    // print_results(dram_latency, l1_latency, l2_latency, l3_latency);
    print_results_for_python(dram_latency, l1_latency, l2_latency, l3_latency);
    free(target_buffer);

    // [1.4] TODO: Uncomment this line once you uncomment the eviction_buffer creation line
    // free(eviction_buffer);
    return 0;
}

