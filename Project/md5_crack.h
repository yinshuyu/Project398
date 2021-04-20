/*Start Header
******************************************************************/
/*!
\file md5_crack.h
\author	Yin shuyu, yin.s, 1802075
\co-author Luo Yu Xuan, yuxuan.luo, 1802205
\par yin.s\@digipen.edu
\co-par yuxuan.luo\@digipen.edu
\date Apr 19, 2021
\brief  CS398 Final Project
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "BruteForceAttack.h"
#include "DictionaryAttack.h"

// CUDA Runtime
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
// Utility and system includes
#include <helper_cuda.h>
#include <helper_functions.h>  // helper for shared that are common to CUDA Samples

#include <stdint.h>
#include <iostream>
#include <vector>
#include <limits>

#include <thread> //multithreading

#include "ThreadDictionaryAttack.h"

#include "GPUBruteForceAttack.h"

//#define DEVPROP
//#define GPU_ONLY

void BruteForceAttack(int argc, char** argv, const char* hash, unsigned msgMinLgth, unsigned msgMaxLgth, unsigned mode, unsigned blockSize, unsigned tileSize)
{
	StopWatchInterface* hTimer = NULL;
	sdkCreateTimer(&hTimer); //Timer Creation

	bool PassFailFlag = false;
	cudaDeviceProp deviceProp;
	deviceProp.major = 0;
	deviceProp.minor = 0;

	// set logfile name and start logs
	printf("[Dictionary Attack] - Starting...\n");

	//Use command-line specified CUDA device, otherwise use device with highest Gflops/s
	int dev = findCudaDevice(argc, (const char**)argv);

	checkCudaErrors(cudaGetDeviceProperties(&deviceProp, dev));

	printf("CUDA device [%s] has %d Multi-Processors, Compute %d.%d\n",
		deviceProp.name, deviceProp.multiProcessorCount, deviceProp.major, deviceProp.minor);

	std::string message = "";

	if (mode != 1)
	{
		std::cout << "CPU Brute Force Attack\n" << std::endl;

		{
			sdkResetTimer(&hTimer);
			sdkStartTimer(&hTimer);

			//sequential brute force attack
			{
				//if bruteforce return true - found password
				PassFailFlag = brute_force(hash, message, msgMinLgth, msgMaxLgth);
			}

			sdkStopTimer(&hTimer);

			//Log CPU execution time 
			{
				printf("...reading back CPU results\n");
				float AvgSecs = 1.0e-3 * (double)sdkGetTimerValue(&hTimer); //sdkgettimer returns millisecs
				printf("CPU_BruteForce() time (average): %.5f sec \n\n", AvgSecs);
			}

			if (PassFailFlag)  //whether the message is found for the sequential dictionary attack
			{
				std::cout << "Hash (" << hash;
				std::cout << ") Cracked: Message (" << message << ")!" << std::endl;
			}
			else
			{
				std::cout << "Failed to crack Hash (" << hash;
				std::cout << ")!" << std::endl;
			}
		}
	}


	char* result = new char[msgMaxLgth + 1];
	memset(result, 0, msgMaxLgth + 1);

	bool found = false;
	if (mode != 2)
	{
		std::cout << "\nGPU brute force attack\n" << std::endl;

		{
			sdkResetTimer(&hTimer);
			sdkStartTimer(&hTimer);

			//transfer data from CPU to GPU
			char* device_hash;
			checkCudaErrors(cudaMalloc((void**)&device_hash, MD5_STRING_SIZE * sizeof(char)));
			checkCudaErrors(cudaMemcpy(device_hash, hash, MD5_STRING_SIZE * sizeof(char), cudaMemcpyHostToDevice));

			char* device_result;
			checkCudaErrors(cudaMalloc((void**)&device_result, msgMaxLgth * sizeof(char)));
			checkCudaErrors(cudaMemcpy(device_result, result, msgMaxLgth * sizeof(char), cudaMemcpyHostToDevice));

			bool* device_found;
			checkCudaErrors(cudaMalloc((void**)&device_found, sizeof(bool)));
			checkCudaErrors(cudaMemcpy(device_found, &found, sizeof(bool), cudaMemcpyHostToDevice));

			//cuda brute force attack
			{
				GPUBruteForce(device_hash, device_result, device_found, msgMinLgth, msgMaxLgth, tileSize, blockSize);
			}

			//transfer result matrix from GPU to CPU
			checkCudaErrors(cudaMemcpy(result, device_result, msgMaxLgth * sizeof(char), cudaMemcpyDeviceToHost));

			checkCudaErrors(cudaMemcpy(&found, device_found, sizeof(bool), cudaMemcpyDeviceToHost));

			checkCudaErrors(cudaFree(device_hash));
			checkCudaErrors(cudaFree(device_result));
			checkCudaErrors(cudaFree(device_found));

			cudaDeviceReset();

			sdkStopTimer(&hTimer);

			//Log GPU execution time 
			{
				printf("...reading back GPU results\n");
				float AvgSecs = 1.0e-3 * (double)sdkGetTimerValue(&hTimer); //sdkgettimer returns millisecs
				printf("GPU_BruteForce() time (average): %.5f sec \n\n", AvgSecs);
			}

			if (found)
			{
				std::cout << "Hash (" << hash;
				std::cout << ") Cracked: Message (" << result << ")!" << std::endl;
			}
			else
			{
				std::cout << "Failed to crack Hash (" << hash;
				std::cout << ")!" << std::endl;
			}
		}
	}

	if (mode == 0)
	{
		if (!message.compare(result))
			std::cout << "GPU and CPU Brute Force attack match." << std::endl;
		else
			std::cout << "GPU and CPU Brute Force attack failed to match!" << std::endl;
	}

	delete[] result;
}

void DictionaryAttack(int argc, char** argv, const char* hash, std::string filename, unsigned fileSize, unsigned msgMaxLgth, unsigned mode, unsigned blockSize, unsigned tileSize)
{
	StopWatchInterface *hTimer = NULL;
	sdkCreateTimer(&hTimer); //Timer Creation

	bool PassFailFlag = false;
	cudaDeviceProp deviceProp;
	deviceProp.major = 0;
	deviceProp.minor = 0;

	// set logfile name and start logs
	printf("[Dictionary Attack] - Starting...\n");

	//Use command-line specified CUDA device, otherwise use device with highest Gflops/s
	int dev = findCudaDevice(argc, (const char **)argv);

	checkCudaErrors(cudaGetDeviceProperties(&deviceProp, dev));

	printf("CUDA device [%s] has %d Multi-Processors, Compute %d.%d\n",
		deviceProp.name, deviceProp.multiProcessorCount, deviceProp.major, deviceProp.minor);

#ifdef DEVPROP
	int driverVersion = 0, runtimeVersion = 0;
	printf("Device %d: \"%s\"\n", dev, deviceProp.name);

	cudaDriverGetVersion(&driverVersion);
	cudaRuntimeGetVersion(&runtimeVersion);
	printf("  CUDA Driver Version / Runtime Version          %d.%d / %d.%d\n",
		driverVersion / 1000, (driverVersion % 100) / 10,
		runtimeVersion / 1000, (runtimeVersion % 100) / 10);
	printf("  CUDA Capability Major/Minor version number:    %d.%d\n",
		deviceProp.major, deviceProp.minor);
	printf("  Total amount of global memory:                 %.2f GBytes (%llu "
		"bytes)\n", (float)deviceProp.totalGlobalMem / pow(1024.0, 3),
		(unsigned long long)deviceProp.totalGlobalMem);
	printf("  GPU Clock rate:                                %.0f MHz (%0.2f "
		"GHz)\n", deviceProp.clockRate * 1e-3f,
		deviceProp.clockRate * 1e-6f);
	printf("  Memory Clock rate:                             %.0f Mhz\n",
		deviceProp.memoryClockRate * 1e-3f);
	printf("  Memory Bus Width:                              %d-bit\n",
		deviceProp.memoryBusWidth);

	if (deviceProp.l2CacheSize)
	{
		printf("  L2 Cache Size:                                 %d bytes\n",
			deviceProp.l2CacheSize);
	}

	printf("  Max Texture Dimension Size (x,y,z)             1D=(%d), "
		"2D=(%d,%d), 3D=(%d,%d,%d)\n", deviceProp.maxTexture1D,
		deviceProp.maxTexture2D[0], deviceProp.maxTexture2D[1],
		deviceProp.maxTexture3D[0], deviceProp.maxTexture3D[1],
		deviceProp.maxTexture3D[2]);
	printf("  Max Layered Texture Size (dim) x layers        1D=(%d) x %d, "
		"2D=(%d,%d) x %d\n", deviceProp.maxTexture1DLayered[0],
		deviceProp.maxTexture1DLayered[1], deviceProp.maxTexture2DLayered[0],
		deviceProp.maxTexture2DLayered[1],
		deviceProp.maxTexture2DLayered[2]);
	printf("  Total amount of constant memory:               %lu bytes\n",
		deviceProp.totalConstMem);
	printf("  Total amount of shared memory per block:       %lu bytes\n",
		deviceProp.sharedMemPerBlock);
	printf("  Total number of registers available per block: %d\n",
		deviceProp.regsPerBlock);
	printf("  Warp size:                                     %d\n",
		deviceProp.warpSize);
	printf("  Maximum number of threads per multiprocessor:  %d\n",
		deviceProp.maxThreadsPerMultiProcessor);
	printf("  Maximum number of threads per block:           %d\n",
		deviceProp.maxThreadsPerBlock);
	printf("  Maximum sizes of each dimension of a block:    %d x %d x %d\n",
		deviceProp.maxThreadsDim[0],
		deviceProp.maxThreadsDim[1],
		deviceProp.maxThreadsDim[2]);
	printf("  Maximum sizes of each dimension of a grid:     %d x %d x %d\n",
		deviceProp.maxGridSize[0],
		deviceProp.maxGridSize[1],
		deviceProp.maxGridSize[2]);
	printf("  Maximum memory pitch:                          %lu bytes\n",
		deviceProp.memPitch);

#endif
//
//	sdkCreateTimer(&hTimer);
//
//	printf("Initializing data...\n");
//	printf("...reading input data\n");
//	printf("...allocating CPU memory.\n");
//
//
//	double dAvgSecs;
//	unsigned byteCount = fileSize * msgMaxLgth *sizeof(char);
//
//	dictionaryList = (char*)malloc(sizeof(char) * byteCount);
//	memcpy_s(dictionaryList, byteCount, h_DataIn, byteCount);
//
//	printf("...allocating GPU memory and copying input data\n\n");
//	checkCudaErrors(cudaMalloc((void **)&d_DataIn, byteCount));
//	checkCudaErrors(cudaMalloc((void **)&d_DataOut, byteCount));
//	checkCudaErrors(cudaMalloc((void **)&d_Mask, 8 * 9 * sizeof(int)));
//	checkCudaErrors(cudaMemcpy(d_DataIn, h_DataIn, byteCount, cudaMemcpyHostToDevice));
//	checkCudaErrors(cudaMemcpy(d_Mask, kirschFilter, 8 * 9 * sizeof(int), cudaMemcpyHostToDevice));
//
//	checkCudaErrors(cudaDeviceSynchronize());
//	sdkResetTimer(&hTimer);
//	sdkStartTimer(&hTimer);
//
//	kirschEdgeDetectorGPU(	d_DataIn, d_Mask, d_DataOut, 
//							imageChannels, imageWidth, imageHeight  );
//
////	printf("\nValidating GPU results...\n");
////	printf(" ...reading back GPU results\n");
//	checkCudaErrors(cudaMemcpy(gpuOutput_image, d_DataOut, byteCount, cudaMemcpyDeviceToHost));
//
//	sdkStopTimer(&hTimer);
//
//	dAvgSecs = 1.0e-3 * (double)sdkGetTimerValue(&hTimer);
//	printf("kirschEdgeDetectionGPU() time (average) : %.5f sec, %.4f MB/sec\n\n", dAvgSecs, ((double)byteCount * 1.0e-6) / dAvgSecs);
//	printf("kirsch edge detection, Throughput = %.4f MB/s, Time = %.5f s, Size = %u Bytes, NumDevsUsed = %u\n",
//		(1.0e-6 * (double)byteCount / dAvgSecs), dAvgSecs, byteCount, 1);
//
//	printf("Shutting down GPU...\n\n");
//
//	checkCudaErrors(cudaFree(d_Mask));
//	checkCudaErrors(cudaFree(d_DataIn));
//	checkCudaErrors(cudaFree(d_DataOut));
//	// cudaDeviceReset causes the driver to clean up all state. While
//	// not mandatory in normal operation, it is good practice.  It is also
//	// needed to ensure correct operation when the application is being
//	// profiled. Calling cudaDeviceReset causes all profile data to be
//	// flushed before the application exits
//	cudaDeviceReset();
//
//	cpuOutput_image = (uchar *)malloc(sizeof(uchar) * byteCount);
//	memcpy_s(cpuOutput_image, byteCount, h_DataIn, byteCount);
//
//	//printf("...kirschEdgeDetectorCPU()\n");
//	sdkResetTimer(&hTimer);
//	sdkStartTimer(&hTimer);
//
//	kirschEdgeDetectorCPU(
//		h_DataIn, (int *) kirschFilter, cpuOutput_image,
//		imageChannels, imageWidth, imageHeight
//	);
//	sdkStopTimer(&hTimer);
//	dAvgSecs = 1.0e-3 * (double)sdkGetTimerValue(&hTimer); 
//	printf("kirschEdgeDetectorCPU() time: %.5f sec, %.4f MB/sec\n\n", dAvgSecs, ((double)byteCount * 1.0e-6) / dAvgSecs);
//	printf("kirschEdgeDetectorCPU, Throughput = %.4f MB/s, Time = %.5f s, Size = %u Bytes\n",
//		(1.0e-6 * (double)byteCount / dAvgSecs), dAvgSecs, byteCount);
//	printf("CPU version done...\n\n");
//
//	sdkDeleteTimer(&hTimer);
//#if 1
//	printf("...comparing the results of GPU and CPU version\n");
//	for (uint i = 0; i < byteCount; i++)
//		if (cpuOutput_image[i] != gpuOutput_image[i])
//		{
//			PassFailFlag = 0;
//			break;
//		}
//	printf(PassFailFlag ? " ...kirsch edge detection matched\n\n" : " ***kirsch edge detection do not match!!!***\n\n");
//#endif

	char* dictionaryList = nullptr;

	//allocate memory for the entire dictionary list
	LoadDictionaryMemory(dictionaryList, fileSize, msgMaxLgth);

	//load in all passwords of the dictionary 
	//into a 1D array - list - dynamically allocated with the msgMaxLength for every password
	if (!LoadDictionary(filename, dictionaryList, msgMaxLgth))
	{
		std::cout << "Failed to Extract Dictionary file passwords!" << std::endl;
		return;
	}
	else
	{
		std::cout << "Dictionary file passwords has been extracted.\n" << std::endl;

	}



#ifndef GPU_ONLY
	
	std::cout << "CPU dictionary attack" << std::endl;

	std::string message = "";

	{

		sdkResetTimer(&hTimer);
		sdkStartTimer(&hTimer);

		//sequential dictionary attack
		{
			//Cracking the User MD5 hash string using the Dictionary attack method
			PassFailFlag = ScanDictionary(hash, message, dictionaryList,
				fileSize, msgMaxLgth);
		}

		sdkStopTimer(&hTimer);
		//Log GPU execution time 
		{
			printf("...reading back GPU results\n");
			float AvgSecs = 1.0e-3 * (double)sdkGetTimerValue(&hTimer); //sdkgettimer returns millisecs
			printf("CPU_ScanDictionary() time (average): %.5f sec, %.4f MB/sec \n",
				AvgSecs, ((double)(fileSize * msgMaxLgth * sizeof(char)) * 1.0e-6) / AvgSecs);
		}

		if (PassFailFlag)  //whether the message is found for the sequential dictionary attack
		{
			std::cout << "Hash (" << hash;
			std::cout << ") Cracked: Message (" << message << ")!" << std::endl;
		}
		else
		{
			std::cout << "Failed to crack Hash (" << hash;
			std::cout << ")!" << std::endl;
		}
	}

#endif

	//cuda dictionary attack
	std::cout << "\nGPU dictionary attack" << std::endl;

	//cuda take in the dictionary list
	//every single thread hash a single password from the dictionary
	//after hashing, compares the user input hash with the dictionary hash

	char* resultArray[3];
	int * index = new int;
	*index = -1;
	char * result = new char[msgMaxLgth];
	memset(result, 0, msgMaxLgth);

	//memset(result, 0, msgMaxLgth);

	{
		sdkResetTimer(&hTimer);
		sdkStartTimer(&hTimer);

		//transfer data from CPU to GPU
		cudaStream_t stream[3];

		char* device_result[3];					//device_result
		char* device_dictionary_list[3]; 		//device dictionary

		char* pinnedMemory_dictionary[3];
		char* pinnedMemory_result[3];


		static bool resultFound[1];
		*resultFound = false;
		bool* deviceresultfound;
		checkCudaErrors(cudaMalloc((void**)&deviceresultfound,
			sizeof(bool)));
		checkCudaErrors(cudaMemcpy(deviceresultfound, resultFound,
			sizeof(bool),
			cudaMemcpyHostToDevice));

		int numberOfStreams = 1;
		int tileSize = 512;

		//device user input hash
		char* device_hash;
		checkCudaErrors(cudaMalloc((void**)&device_hash,
			MD5_STRING_SIZE * sizeof(char)));
		checkCudaErrors(cudaMemcpy(device_hash, hash,
			MD5_STRING_SIZE * sizeof(char),
			cudaMemcpyHostToDevice));

		for (int i = 0; i < numberOfStreams + 1; i++)
		{
			resultArray[i] = new char[msgMaxLgth];
			memset(resultArray[i], 0, msgMaxLgth);

			//Create 3 streams
			cudaStreamCreate(&stream[i]);

			checkCudaErrors(cudaMalloc((void**)&device_result[i],
				msgMaxLgth * sizeof(char)));
			//transfer nullptr to device memory
			checkCudaErrors(cudaMemcpy(device_result[i], result,
				msgMaxLgth * sizeof(char),
				cudaMemcpyHostToDevice));

			checkCudaErrors(cudaMalloc((void**)&device_dictionary_list[i],
				tileSize * tileSize * msgMaxLgth * sizeof(char)));

			//Allocate pointers with Pinned Memory for CPU
			cudaHostAlloc((void**)&pinnedMemory_dictionary[i],
				tileSize * tileSize * msgMaxLgth * sizeof(char), 
				cudaHostAllocDefault);

			cudaHostAlloc((void**)&pinnedMemory_result[i],
				msgMaxLgth * sizeof(char),
				cudaHostAllocDefault);
		}

		sdkStopTimer(&hTimer);
		//Log GPU execution time 
		{
			float AvgSecs = 1.0e-3 * (double)sdkGetTimerValue(&hTimer); //sdkgettimer returns millisecs
			printf("GPU Memory Allocation Time time (average): %.5f sec\n",
				AvgSecs);
		}



		{

			sdkResetTimer(&hTimer);
			sdkStartTimer(&hTimer);

			std::thread first(thread1_function,

				msgMaxLgth, //max message length of each dictionary word
				fileSize, //size of dictionary
				dictionaryList, //host dictionary memory

				pinnedMemory_dictionary,  //transfer the host dictionary to the pinnedMemory

				numberOfStreams, //0 to 2, 3 streams

				tileSize //default tilesize is 512 
			);


			std::thread second (thread2_function,

				msgMaxLgth, //max message length of each dictionary word
				fileSize, //size of dictionary

				device_hash,
				device_result,
				device_dictionary_list, 

				pinnedMemory_dictionary,  //transfer the host dictionary to the pinnedMemory
				pinnedMemory_result,

				resultArray, //host memory for result
				index,
				stream,
				numberOfStreams, //0 to 2, 3 streams

				tileSize, //default tilesize is 512 , 
				deviceresultfound

			);


			first.join();
			second.join();

			sdkStopTimer(&hTimer);
			//Log GPU execution time 
			{
				printf("...reading back GPU results\n");
				float AvgSecs = 1.0e-3 * (double)sdkGetTimerValue(&hTimer); //sdkgettimer returns millisecs
				printf("GPU Execution Time: GPUScanDictionary() time (average): %.5f sec, %.4f MB/sec \n",
					AvgSecs, ((double)(fileSize * msgMaxLgth * sizeof(char)) * 1.0e-6) / AvgSecs);
			}
		}


		if (*index > -1)
		{
			result = resultArray[*index];
		}
		else
		{
			result = 0;
		}

		if (result)
		{
			std::cout << "Hash (" << hash;
			std::cout << ") Cracked: Message (" << result << ")!\n" << std::endl;
		}
		else
		{
			std::cout << "Failed to crack Hash (" << hash;
			std::cout << ")!" << std::endl;
		}

#ifndef GPU_ONLY


		if (!message.compare(result))
		{
			std::cout << "GPU and CPU dictionary attack match." << std::endl;

		}
		else
		{
			std::cout << "GPU and CPU dictionary attack failed to match!" << std::endl;
		}

#endif 

		{
			printf("Shutting down...\n");
			FreeDictionaryMemory(dictionaryList);

			//free CPU allocated memory
			printf("freeing host Data Input for GPU version...\n");

			for (int i = 0; i < numberOfStreams + 1; i++)
			{

				//free cuda device allocated memory
				checkCudaErrors(cudaFree(device_result[i]));
				checkCudaErrors(cudaFree(device_dictionary_list[i]));

				//Free pinned Memory
				cudaFreeHost(pinnedMemory_dictionary[i]);
				cudaFreeHost(pinnedMemory_result[i]);

				//destroy streams
				cudaStreamDestroy(stream[i]);
			}

			cudaDeviceReset();
		}
	}






}