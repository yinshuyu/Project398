#pragma once

//Used in GPU mode
//Calls kernel function
extern "C" void GPUBruteForce(
	char* hash,  //user input hash
	char* result,      //password result found by kernel
	bool* found,

	unsigned msgMinLgth,	//the min search length
	unsigned msgMaxLgth,	//the max search length

	unsigned tileSize
);