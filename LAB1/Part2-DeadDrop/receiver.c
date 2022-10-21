
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>
#define ThreshHold 65
#define ThreshHold2 35
// #define samples 80000 //Was working
// #define samples 40000
#define samples 60000
#define StartWait 128
int main(int argc, char **argv)
{
	// Put your covert channel setup code here
	uint8_t *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);

	*buf=0;

	printf("Please press enter.\n");

    size_t bytesToFill = 32768;// + 262144;
	
    size_t amtOfThings = bytesToFill / sizeof(uint64_t**);
    uint64_t** test = (uint64_t**) malloc(bytesToFill);
    
    
    for (size_t i = 0; i < amtOfThings; i++)
        test[i] =  (uint64_t*) test + (i+1);
    
    test[amtOfThings -1] = (uint64_t*) test;
    
    volatile uint64_t** pnt = (volatile uint64_t**) test;


	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);

	printf("Receiver now listening.\n");
	measure_one_block_access_time((uint64_t) buf + (TransmittingSignal<<6));
	measure_one_block_access_time((uint64_t) buf + (TransmittingSignal<<6));
	measure_one_block_access_time((uint64_t) buf + (TransmittingSignal<<6));
	bool listening = true;
	uint32_t PastLatency[4] = {134217727,134217727,134217727,134217727};
	uint8_t tmpCnt=0;
	while (listening) {
		int output = 0;
		if (tmpCnt<=StartWait) ++tmpCnt;
		
		PastLatency[0] = PastLatency[1];
		PastLatency[1] = PastLatency[2];
		PastLatency[2] = PastLatency[3];
		// PastLatency[3] = PastLatency[4];
		// PastLatency[4] = PastLatency[5];
		for (size_t j = StartIndex; j < StopIndex; j++)
		{
			measure_one_block_access_time(((uint64_t) buf) + (j<<6));
			measure_one_block_access_time(((uint64_t) buf) + ((j+OffsetA)<<6));
			measure_one_block_access_time(((uint64_t) buf) + ((j+OffsetB)<<6));
			// measure_one_block_access_time(((uint64_t) buf) + ((j+OffsetC)<<6));
			// measure_one_block_access_time(((uint64_t) buf) + ((j+OffsetD)<<6));
		}


		PastLatency[3] = measure_one_block_access_time((uint64_t) buf + (TransmittingSignal<<6));
		for (size_t j = 0; j < amtOfThings*10; j++)
              pnt = (volatile uint64_t**) *pnt;

		if(PastLatency[3]>100) PastLatency[3] = PastLatency[2];

		int average = (PastLatency[0]+PastLatency[1]+PastLatency[2]+PastLatency[3])/4;
		// printf("\raverage: %d", average);
		// fflush(stdout);
		if (average > ThreshHold && average < 500 && tmpCnt >100)
		{

			// printf("average: %d, tmpCnt: %ld\n",average,tmpCnt);
			uint32_t cnt[]  = {0,0,0,0,0,0,0,0};
			uint32_t cntA[] = {0,0,0,0,0,0,0,0};
			uint32_t cntB[] = {0,0,0,0,0,0,0,0};
			// uint32_t cntC[] = {0,0,0,0,0,0,0,0};
			// uint32_t cntD[] = {0,0,0,0,0,0,0,0};
			for (size_t i = 0; i < samples; i++)
			{
				for (size_t j = 0; j < amtOfThings*1; j++)
              		pnt = (volatile uint64_t**) *pnt;


				for (size_t j = StartIndex; j < StopIndex; j++)
				{
					// printf("%d ",j);
					cnt[j-StartIndex] +=  measure_one_block_access_time(((uint64_t) buf) + (j<<6));
					cntA[j-StartIndex] +=  measure_one_block_access_time(((uint64_t) buf) + ((j+OffsetA)<<6));
					cntB[j-StartIndex] +=  measure_one_block_access_time(((uint64_t) buf) + ((j+OffsetB)<<6));
					// cntC[j-StartIndex] +=  measure_one_block_access_time(((uint64_t) buf) + ((j+OffsetC)<<6));
					// cntD[j-StartIndex] +=  measure_one_block_access_time(((uint64_t) buf) + ((j+OffsetD)<<6));
					
				}
				// printf("\n");
				
				
			}
			// printf("\nCNT: %d %d %d %d %d %d %d %d\n",
			//  cnt[0]/samples , cnt[1]/samples , cnt[2]/samples, cnt[3]/samples , cnt[4]/samples , cnt[5]/samples , cnt[6]/samples, cnt[7]/samples );
			
			// printf("\nCNTA: %d %d %d %d %d %d %d %d\n",
			//  cntA[0]/samples , cntA[1]/samples , cntA[2]/samples, cntA[3]/samples , cntA[4]/samples , cntA[5]/samples , cntA[6]/samples, cntA[7]/samples );

			// printf("\nCNTB: %d %d %d %d %d %d %d %d\n",
			//  cntB[0]/samples , cntB[1]/samples , cntB[2]/samples, cntB[3]/samples , cntB[4]/samples , cntB[5]/samples , cntB[6]/samples, cntB[7]/samples );


			for (size_t i = 0; i < 8; i++)
			{
				// if (cnt[i]>300 ||cntA[i]>300 ||cntB[i]>300 )//||cntC[i]>300 ||cntD[i]>300)
				// {
				// 	printf("i: %d, %d %d %d\n",i,cnt[i],cntA[i],cntB[i]);
				// }
				
				// output |= (cnt[i]/samples > ThreshHold2) << i;
				// output |= (((cnt[i]/samples > ThreshHold2) + (cntA[i]/samples > ThreshHold2) + (cntB[i]/samples > ThreshHold2) + (cntC[i]/samples > ThreshHold2)+ (cntD[i]/samples > ThreshHold2)) > 2 ) << i;
				output |= (((cnt[i]/samples > ThreshHold2) + (cntA[i]/samples > ThreshHold2) + (cntB[i]/samples > ThreshHold2)) >= 2 ) << i;
			}
			PastLatency[0] = PastLatency[1] = PastLatency[2] = PastLatency[3] = ThreshHold-1;
			// wait(500*__builtin_popcount(output));
			printf("\nOutput: %d\n", output);
			
			
		}
	}

	printf("Receiver finished.\n");

	return 0;
}


