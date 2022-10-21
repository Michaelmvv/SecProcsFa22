
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>

// TODO: define your own buffer size
//#define BUFF_SIZE [TODO]

// int getBit(int num, int bit){
//   // return (num & (0b1<<bit)) >> bit;
//   return bit 
// }

int main(int argc, char **argv)
{
  // Allocate a buffer using huge page
  // See the handout for details about hugepage management
  uint8_t *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
  // volatile int64_t tmp;
  if (buf == (void*) - 1) {
     perror("mmap() error\n");
     exit(EXIT_FAILURE);
  }
  // The first access to a page triggers overhead associated with
  // page allocation, TLB insertion, etc.
  // Thus, we use a dummy write here to trigger page allocation
  // so later access will not suffer from such overhead.
  //*((char *)buf) = 1; // dummy write to trigger page allocation
  buf[0]=0;

  // TODO:
  // Put your covert channel setup code here

  // printf("Please type a message.\n");

    size_t bytesToFill = 32768;
    size_t amtOfThings = bytesToFill / sizeof(uint64_t**);
    uint64_t** test = (uint64_t**) malloc(bytesToFill);
    
    
    for (size_t i = 0; i < amtOfThings; i++)
        test[i] =  (uint64_t*) test + (i+1);
    
    test[amtOfThings -1] = (uint64_t*) test;
    
    volatile uint64_t** pnt = (volatile uint64_t**) test;




  bool sending = true;
  while (sending) {

      char text_buf[128];
      printf("Please type a message.\n");
      fgets(text_buf, sizeof(text_buf), stdin);
      if (strlen(text_buf) == 1) continue;
      
      
      int num = string_to_int(text_buf);
      // int bit0 = num & 1;
      // int bit1 = (num>>1) & 1;
      // int bit2 = (num>>2) & 1;
      // int bit3 = (num>>3) & 1;
      // int bit4 = (num>>4) & 1;
      // int bit5 = (num>>5) & 1;
      // int bit6 = (num>>6) & 1;
      // int bit7 = (num>>7) & 1;

      //800000 Was working
      //400000 was working
      for (size_t i = 0; i < 400000; i++)
      {
        
        for (size_t j = StartIndex; j < StopIndex; j++)
        {
          
            // Different tags Tags
            for (size_t k = 0; k < 15; k++)
            {
              // int64_t address = k << 15 | j<<6;
	      //for (size_t l = 0; l<2; l++)
              if((num>>j) & 1) {
                *((uint8_t*)(((uint64_t) buf) + (k << 15 | j<<6))) = j;
                *((uint8_t*)(((uint64_t) buf) + (k << 15 | (j+OffsetA)<<6))) = j;
                *((uint8_t*)(((uint64_t) buf) + (k << 15 | (j+OffsetB)<<6))) = j;
                // *((uint8_t*)(((uint64_t) buf) + (k << 15 | (j+OffsetC)<<6))) = j;
                // *((uint8_t*)(((uint64_t) buf) + (k << 15 | (j+OffsetD)<<6))) = j;
              }
              // else{
              //   // wait(30);
              // }
              
              *((uint8_t*)(((uint64_t) buf) + (k << 15 | TransmittingSignal<<6))) = j; //Indicate sending
            }
            
            // for (size_t j = 0; j < amtOfThings*10; j++)
            //   pnt = (volatile uint64_t**) *pnt;
        }
        // wait(900);
        
      }
      // sending =false;

      //Do not send messages faster than can be recived. 
      wait(1000000);
      
  }
  printf("Sender finished.\n");
  return 0;
}


