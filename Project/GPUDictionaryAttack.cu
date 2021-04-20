/*Start Header
******************************************************************/
/*!
\file GPUDictionaryAttack.cu
\author Luo Yu Xuan, yuxuan.luo, 1802205
\par yuxuan.luo\@digipen.edu
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
#include "md5.h"
#include <math.h>


#define BLOCK_SIZE 32
typedef unsigned int uint;



__global__ void Kernel_ScanDictionary(
	const char* __restrict__ hash,
	char * list, 
	unsigned listSize,
	char* result, 
	unsigned msgMaxLgth,
	bool * resultfound
)
{

	int bx = blockIdx.x;
	int by = blockIdx.y;
	int tx = threadIdx.x;
	int ty = threadIdx.y;
	int col = bx * blockDim.x + tx;
	int row = by * blockDim.y + ty;

	int x_size = gridDim.x * blockDim.x;
	int size = row * x_size + col;

	if (size < listSize)
	{
		char chash[MD5_STRING_SIZE];

		unsigned length = 0;

		while (list[size *msgMaxLgth + length] != '\0')
			length++;


		//hash a single dictionary password to MD5hash format
		md5::md5_t md5;
		md5.process(&(list[size * msgMaxLgth]), length);
		md5.finish();
		md5.get_string(chash);

		//compare if the dictionary MD5hash format is the same as the user input
		int i = 0;
		bool match = true;
		while (i < MD5_STRING_SIZE)
		{
			if (chash[i] != hash[i])
			{
				match = false;
				break;
			}
			i++;
		}

		if (match)
		{
			//convert the dictionary char * to std::string
			//result = &(list[size * msgMaxLgth]);

			//memcpy(result, &(list[size * msgMaxLgth]), length);

			*resultfound = true;
			for (int i = 0; i < length; i++)
			{
				result[i] = list[size * msgMaxLgth + i];
			}
			//found the password in the dictionary
		}
	}

}




//Used in GPU mode
//Calls kernel function
extern "C" void GPUScanDictionary(
	char* hash,  //user input hash
	char* result,      //password result found by kernel

	char* list,			 //dictionary list
	unsigned listSize,	//size of dictionary
	unsigned msgMaxLgth, //the max length of a password in the dictionary

	unsigned tileSize,
	cudaStream_t stream, //stream number, 
	bool * resultfound
)
{
	uint block = BLOCK_SIZE;

	dim3 blockDim(block, block, 1); //32 by 32

	int y_size = ceil((float)listSize / (float)tileSize);
	dim3 gridDim(ceil((float)(tileSize) / (float)block), ceil((float)(y_size) / (float)block), 1);

	Kernel_ScanDictionary << <gridDim, blockDim , 0, stream>> > (
		hash,
		list,
		listSize,
		result,
		msgMaxLgth, resultfound);

	getLastCudaError("Kernel_ScanDictionary failed\n");

	cudaDeviceSynchronize();
}