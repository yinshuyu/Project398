#pragma once



#ifndef GPU_DICTIONARYATTACK_H
#define GPU_DICTIONARYATTACK_H


//Used in GPU mode
//Calls kernel function
extern "C" void GPUScanDictionary(
	char* hash,  //user input hash
	char* result,      //password result found by kernel

	char* list,			 //dictionary list
	unsigned listSize,	//size of dictionary
	unsigned msgMaxLgth, //the max length of a password in the dictionary

	unsigned tileSize
	//cudaStream_t stream //stream number
);


#endif
