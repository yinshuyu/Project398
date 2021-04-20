/*Start Header
******************************************************************/
/*!
\file ThreadDictionaryAttack.h
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
#include "Semaphore.h"
#include "GPUDictionaryAttack.h"

#include <helper_functions.h>
#include <helper_cuda.h>
#include "device_launch_parameters.h"
#include "cuda_runtime.h"


typedef unsigned int uint;

//3 semaphores for each thread
static Semaphore semaphores[2][3];

//std::mutex result_mutex;
static bool * resultFound = new bool;


//function to copy host dictionary memory to host pinnedmemory
static void thread1_function(

	unsigned int msgMaxLgth, //max message length of each dictionary word
	unsigned int dictionary_size, //size of dictionary
	char* dictionary_list, //host dictionary memory

	char* pinnedMemory_dictionary[3],  //transfer the host dictionary to the pinnedMemory

	int numOfStreams, //0 to 2, 3 streams

	uint tileSize//default tilesize is 512 
){
	*resultFound = false;

	int stream_id = 0;
	bool init_once_stream[3];

	for (int i = 0; i < 3; i++)
		init_once_stream[i] = true;


	bool first_loop = true;

	
	for (uint i = 0; i < dictionary_size; i += tileSize * tileSize)
	{
	
		//	select a stream
		//	this stream wait for host pinned memory to be available
		if (init_once_stream[stream_id])
			init_once_stream[stream_id] = false;
		else
		{

			semaphores[0][stream_id].wait();

			if (*resultFound) //if the result is already found
			{
				return;
			}
		}
	
		//if tileSize * tileSize exceeds the dictionary size
		int size = (dictionary_size < i + tileSize * tileSize) ? 
			dictionary_size - i : tileSize * tileSize;

		memcpy(pinnedMemory_dictionary[stream_id],
			dictionary_list + (i * msgMaxLgth),		
			size * msgMaxLgth * sizeof(char));
	
	
		semaphores[1][stream_id].signal();
	
		//allocate job to the next stream
		if (stream_id == numOfStreams)
			stream_id = 0;
		else
			stream_id++;
	}
	
}

//function: to copy pinned memory to device memory
//			call kernel to run
//			copy result back from device memory to pinned memory
static void thread2_function(

	unsigned int msgMaxLgth, //max message length of each dictionary word
	unsigned int dictionary_size, //size of dictionary

	char* device_hash,
	char* device_result[3],
	char* device_dictionary[3], 

	char* pinnedMemory_dictionary[3],  //transfer the host dictionary to the pinnedMemory
	char* pinnedMemory_result[3],
	
	char* final_result[3], //host memory for result
	int * index,
	cudaStream_t stream[3],
	int numOfStreams, //0 to 2, 3 streams

	int tileSize//default tilesize is 512
	, bool * deviceresultfound, 
	unsigned int blockSize
	) //default number of streams is 3 streams
{

	int stream_id = 0;

	cudaEvent_t H2D_Finished;
	cudaEventCreate(&H2D_Finished);

	cudaEvent_t Kernel_Calc_Finished;
	cudaEventCreate(&Kernel_Calc_Finished);

	cudaEvent_t D2H_Finished;
	cudaEventCreate(&D2H_Finished);

	for (uint i = 0; i < dictionary_size; i += tileSize * tileSize)
	{

		semaphores[1][stream_id].wait();

		int size = (dictionary_size < i + tileSize * tileSize) ? dictionary_size - i : tileSize * tileSize;

		//copy from host pinned memory to device memory
		checkCudaErrors(
			cudaMemcpyAsync(device_dictionary[stream_id], 
				pinnedMemory_dictionary[stream_id],
			size * msgMaxLgth * sizeof(char),
			cudaMemcpyHostToDevice, 
				stream[stream_id]));
		

		GPUScanDictionary(
			device_hash, 
			device_result[stream_id],
			device_dictionary[stream_id],
			size, 
			msgMaxLgth, 
			tileSize, 
			stream[stream_id], deviceresultfound, 
			blockSize);


		checkCudaErrors(cudaMemcpyAsync(
			(void*) resultFound, deviceresultfound,
			 sizeof(bool), cudaMemcpyDeviceToHost), stream[stream_id]);




		if (*resultFound) //if the result is already found
		{

			checkCudaErrors(cudaMemcpy(
				final_result[stream_id], device_result[stream_id],
				msgMaxLgth * sizeof(char), cudaMemcpyDeviceToHost));

			*index = stream_id;

			for(int i = 0; i < numOfStreams + 1; i++) //signal the first thread and all streams
				semaphores[0][i].signal();

			return;
		}

		semaphores[0][stream_id].signal();

		//allocate job to the next stream
		if (stream_id == numOfStreams)
			stream_id = 0;
		else
			stream_id++;
	}
}

