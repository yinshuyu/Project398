/*Start Header
******************************************************************/
/*!
\file GPUBruteForceAttack.h
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

#pragma once

//Used in GPU mode
//Calls kernel function
extern "C" void GPUBruteForce(
	char* hash,  //user input hash
	char* result,      //password result found by kernel
	bool* found,

	unsigned msgMinLgth,	//the min search length
	unsigned msgMaxLgth,	//the max search length

	unsigned tileSize,
	unsigned blockSize
);