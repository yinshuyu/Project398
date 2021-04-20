/*Start Header
******************************************************************/
/*!
\file GPUDictionaryAttack.h
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
#pragma once

#ifndef GPU_DICTIONARYATTACK_H
#define GPU_DICTIONARYATTACK_H

#include "cuda_runtime.h"

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
	bool * resultfound, 
	unsigned int blockSize
);


#endif
