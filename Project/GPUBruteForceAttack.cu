/*Start Header
******************************************************************/
/*!
\file GPUBruteForceAttack.cu
\author	Yin shuyu, yin.s, 1802075
\par yin.s\@digipen.edu
\date Apr 19, 2021
\brief  CS398 Final Project
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

// Utility and system includes
#include <helper_cuda.h>
#include <math.h>
#include "md5.h"
#include "BruteForceAttack_loc.h"

typedef unsigned int uint;

__global__ void Kernel_BruteForce(
	const char* __restrict__ hash,
	char* result,
	bool* found,
	unsigned msgLgth,
	unsigned startIndex,
	unsigned endIndex
)
{
	int blockId = blockIdx.x + blockIdx.y * gridDim.x;
	int threadId = blockId * (blockDim.x * blockDim.y)
		+ (threadIdx.y * blockDim.x) + threadIdx.x;

	// thread index -- also the combination's index
	unsigned index = startIndex + threadId;

	if (index < endIndex)
	{
		char* test = new char[msgLgth];

		// get the test msg for this thread
		{
			unsigned dividend = index;
			unsigned quotient = 0;
			//unsigned remainder = 0;

			unsigned i = 0;
			for (unsigned divisor = endIndex/ RANGE; i < msgLgth - 1; i++, divisor /= RANGE)
			{
				quotient = floor((float)(dividend) / (float)divisor);
				//	remainder = dividend % divisor;
				dividend %= divisor;

				test[i] = START + quotient;

				//	dividend = remainder;
			}

			test[i] = START + dividend;

			
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


// n^k possible strings of length k that can be formed from a set of n characters
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

	unsigned tileSize,
	unsigned blockSize
)
{
	unsigned maxTileSize = tileSize * tileSize;
	//printf("%d\n\n", maxTileSize);
	bool hashfound; // use to break when hash in find in one of the kenrel

	// split the all combination in different msg length batch 
	for (unsigned msgLgth = msgMinLgth; msgLgth <= msgMaxLgth; msgLgth++)
	{
		// number combination in this ASCII range and msg length
		unsigned endIndex = possibleAmount(RANGE, msgLgth);
		//printf("%d\n\n", endIndex);

		for (unsigned startIndex = 0; startIndex < endIndex; startIndex += maxTileSize)
		{
			unsigned remainderIndex = endIndex - startIndex;
			unsigned y_size = tileSize;

			if(maxTileSize > remainderIndex)
				y_size = ceil((float)remainderIndex / (float)tileSize);

			//printf("%d\n", startIndex);
			//printf("%d\n", y_size);

			dim3 blockDim(blockSize, blockSize, 1); //32 by 32

			dim3 gridDim(ceil((float)(tileSize) / (float)blockSize), ceil((float)(y_size) / (float)blockSize), 1);

			Kernel_BruteForce << <gridDim, blockDim >> > (
				hash,
				result,
				found,
				msgLgth,
				startIndex,
				endIndex);

			cudaMemcpy(&hashfound, found, sizeof(bool), cudaMemcpyDeviceToHost);

			getLastCudaError("Kernel_BruteForce failed\n");

			cudaDeviceSynchronize();

			// stop if hash is found
			if (hashfound)
				return;
		}	
	}
}