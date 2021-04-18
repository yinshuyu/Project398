

#include "Semaphore.h"
#include "GPUDictionaryAttack.h"

#include <helper_functions.h>
#include <helper_cuda.h>
#include "device_launch_parameters.h"
#include "cuda_runtime.h"


typedef unsigned int uint;

//3 semaphores for each thread
static Semaphore semaphores[2][3];

static bool resultFound = false;


//function to copy host dictionary memory to host pinnedmemory
static void thread1_function(

	unsigned int msgMaxLgth, //max message length of each dictionary word
	unsigned int dictionary_size, //size of dictionary
	char* dictionary_list, //host dictionary memory

	char* pinnedMemory_dictionary[3],  //transfer the host dictionary to the pinnedMemory

	int numOfStreams, //0 to 2, 3 streams

	uint tileSize//default tilesize is 512 
) //default number of streams is 3 streams
{

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
			//semaphores[0][stream_id]++; //increment semaphore by 1
		//{
		//	std::unique_lock<std::mutex> lk(mutex[0][stream_id]);
		//	while (!flag[0][stream_id])
		//	{
		//		std::cout << "waiting 1:   " << stream_id << std::endl;
		//		semaphores[0][stream_id].wait(lk);
		//	}
		//	flag[0][stream_id] = false;
		//	std::cout << "Stop waiting 1:    " << stream_id << std::endl;
		//}
			semaphores[0][stream_id].wait();
			//std::cout << "Stop waiting 1:    " << stream_id << std::endl;
	
			//wait for semaphore to become 0 
			//while (semaphores[0][stream_id] != 0);

			if (resultFound) //if the result is already found
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
	
	
		//	signal to thread 2
		//semaphores[1][stream_id]--;
		//flag[1][stream_id] = true;
		//semaphores[1][stream_id].notify_all();
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

		//select a stream
		//this stream wait for host pinned memory to be available

		//semaphores[1][stream_id]++; //increment semaphore by 1
		//while (semaphores[1][stream_id] != 0); //wait for semaphore to become 0 
		//std::unique_lock<std::mutex> lk(mutex[1][stream_id]);
		//while (!flag[1][stream_id])
		//{
		//	std::cout << "waiting 2:  " << stream_id << std::endl;
		//	semaphores[1][stream_id].wait(lk);
		//}
		//std::cout << "stop waiting 2:   " << stream_id << std::endl;
		//flag[1][stream_id] = false;
		semaphores[1][stream_id].wait();
		//std::cout << "Stop waiting 2:    " << stream_id << std::endl;

		int size = (dictionary_size < i + tileSize * tileSize) ? dictionary_size - i : tileSize * tileSize;

		//copy from host pinned memory to device memory
		checkCudaErrors(
			cudaMemcpyAsync(device_dictionary[stream_id], 
				pinnedMemory_dictionary[stream_id],
			size * msgMaxLgth * sizeof(char),
			cudaMemcpyHostToDevice, 
				stream[stream_id]));
		


		//generate event H2D finished for this stream
		//this stream wait for H2D finished
		checkCudaErrors(cudaEventRecord(H2D_Finished, stream[stream_id]));


		//launch kernel to compute tiled matrix multiplication
		GPUScanDictionary(
			device_hash, 
			device_result[stream_id],
			device_dictionary[stream_id],
			size, 
			msgMaxLgth, 
			tileSize, 
			stream[stream_id]);

		//generate event Calc kernel finished for this stream
		//this stream wait for Calc kernel finished
		checkCudaErrors(cudaEventRecord(Kernel_Calc_Finished, stream[stream_id]));


		//copy result tile C from device memory to host pinned memory
		//checkCudaErrors(cudaMemcpyAsync(
		//	pinnedMemory_result[stream_id], device_result[stream_id],
		//	msgMaxLgth * sizeof(char), cudaMemcpyDeviceToHost, 
		//	stream[stream_id]));


		checkCudaErrors(cudaMemcpy(
			final_result[stream_id], device_result[stream_id],
			msgMaxLgth * sizeof(char), cudaMemcpyDeviceToHost));

	//std::cout << hostresult << std::endl;

		//generate event D2H finished for this stream
		checkCudaErrors(cudaEventRecord(D2H_Finished, stream[stream_id]));
		//for (int i = 0; i < 3; i++)
		//{
		//	if (i == stream_id)
		//		continue;
		//
		//	checkCudaErrors(cudaStreamWaitEvent(stream[i], D2H_Finished, 0));
		//}

		if (final_result[stream_id]) //if the result is already found
		{
			//memcpy(final_result, pinnedMemory_result[stream_id],
			//	msgMaxLgth * sizeof(char)); //get the result

			*index = stream_id;
			resultFound = true;  

			for(int i = 0; i < numOfStreams + 1; i++) //signal the first thread and all streams
				semaphores[0][i].signal();

			return;
		}

		//signal to thread 3
		//semaphores[2][stream_id]--;
		//flag[2][stream_id] = true;
		//semaphores[2][stream_id].notify_all();
		//semaphores[2][stream_id].signal();

		semaphores[0][stream_id].signal();

		//allocate job to the next stream
		if (stream_id == numOfStreams)
			stream_id = 0;
		else
			stream_id++;
	}
}

