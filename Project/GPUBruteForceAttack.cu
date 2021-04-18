// Utility and system includes
#include <helper_cuda.h>
#include <math.h>
#include "md5.h"
#include "BruteForceAttack_loc.h"

#define BLOCK_SIZE 32
typedef unsigned int uint;

// n^k possible strings of length k that can be formed from a set of n characters
__global__ void Kernel_BruteForce(
	const char* __restrict__ hash,
	char* result,
	bool* found,
	unsigned msgLgth,
	unsigned startIndex,
	unsigned endIndex
)
{
	unsigned bx = blockIdx.x;
	unsigned by = blockIdx.y;
	unsigned tx = threadIdx.x;
	unsigned ty = threadIdx.y;
	unsigned col = bx * blockDim.x + tx;
	unsigned row = by * blockDim.y + ty;

	unsigned index = startIndex + (row * blockDim.x + col);

	if (index < endIndex)
	{
		char* test = new char[msgLgth];

		// get the test msg for this thread
		{
			unsigned dividend = index;
			unsigned quotient = 0;
			//unsigned remainder = 0;

			unsigned i = 0;
			for (unsigned divisor = endIndex/RANGE_0; i < msgLgth - 1; i++, divisor /= RANGE_0)
			{
				quotient = floor((float)(dividend) / (float)divisor);
				//	remainder = dividend % divisor;
				dividend %= divisor;

				test[i] = START_0 + quotient;

				//	dividend = remainder;
			}

			test[i] = START_0 + dividend;

			
		}

		__syncthreads();

		char chash[MD5_STRING_SIZE];

		//hash a single dictionary password to MD5hash format
		md5::md5_t md5;
		md5.process(test, msgLgth);
		md5.finish();
		md5.get_string(chash);

		__syncthreads();

		//compare if the dictionary MD5hash format is the same as the user input
		bool match = true;

		for (unsigned i = 0; i < MD5_STRING_SIZE; i++)
		{
			if (chash[i] != hash[i])
			{
				match = false;
				break;
			}
		}

		if (match)
		{
			*found = true;

			//memcpy(result, &(list[size * msgMaxLgth]), length);
			for (unsigned i = 0; i < msgLgth; i++)
				result[i] = test[i];
		}

		__syncthreads();

		delete[] test;
	}

}


// A function to find the factorial.
int possibleAmount(int n, int k)
{
	int result = 1;

	for (int i = 0; i < k; i++)
		result *= n;

	return result;
}

//Used in GPU mode
//Calls kernel function
extern "C" void GPUBruteForce(
	char* hash,		//user input hash
	char* result,	//password result found by kernel
	bool* found,

	unsigned msgMinLgth,	//the min search length
	unsigned msgMaxLgth,	//the max search length

	unsigned tileSize
)
{
	unsigned maxTileSize = tileSize * tileSize;
	//printf("%d\n\n", maxTileSize);

	for (unsigned msgLgth = msgMinLgth; msgLgth <= msgMaxLgth; msgLgth++)
	{
		unsigned endIndex = possibleAmount(RANGE_0, msgLgth);
		//printf("%d\n\n", endIndex);

		for (unsigned startIndex = 0; startIndex < endIndex; startIndex += maxTileSize)
		{
			unsigned remainderIndex = endIndex - startIndex;
			unsigned y_size = tileSize;

			if(maxTileSize > remainderIndex)
				y_size = ceil((float)remainderIndex / (float)tileSize);

			//printf("%d\n", startIndex);
			//printf("%d\n", y_size);

			dim3 blockDim(BLOCK_SIZE, BLOCK_SIZE, 1); //32 by 32

			dim3 gridDim(ceil((float)(tileSize) / (float)BLOCK_SIZE), ceil((float)(y_size) / (float)BLOCK_SIZE), 1);

			Kernel_BruteForce << <gridDim, blockDim >> > (
				hash,
				result,
				found,
				msgLgth,
				startIndex,
				endIndex);

			getLastCudaError("Kernel_BruteForce failed\n");

			cudaDeviceSynchronize();

			/*if (found)
				return;*/
		}	
	}
}