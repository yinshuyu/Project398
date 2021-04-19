#pragma once
#include <cstdint>
#include "md5.h"
#include "BruteForceAttack_loc.h"


typedef unsigned char byte_t;

int compare(const uint32_t a[], const uint32_t b[]) {
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
}


//recursive function
int brute_force_fixed_length(const char* hash, std::string& result, 
	char* test, //ascii value that is being tested in the function 
	int p,	 //index of the test array 
	int len  //find all the permuations of length len
	) 
	{

	static char chash[MD5_STRING_SIZE];

	char c;

	if (p < len) {
		for (c = START_0; c <= END_9; ++c) {
			test[p] = c;
			if (brute_force_fixed_length(hash, result, test, p + 1, len))
				return 1;
		}
	}
	else { //reached the desired length of permutation
		for (c = START_0; c <= END_9; ++c) {
			test[p] = c;

			//get hash of the string
			md5::md5_t md5;
			md5.process(test, len + 1);
			md5.finish();

			md5.get_string(chash);

			std::string s(chash);

			//compare user input hash and permutation hash
			if (!s.compare(hash))
			{
				result = test;
				return 1;
			}
		}
	}

	return 0;
}

bool brute_force(const char* hash, std::string& result, unsigned minlen, unsigned maxlen) {

	char* str = new char[maxlen+1];

	for (unsigned i = 0; i < maxlen+1; i++)
		str[i] = '\0';

	//eg.minlen = 3, maxlen = 6 
	//3 to 6 - 4 characters
	//find the permutations between minlen to maxlen
	for (unsigned i = 0; i < maxlen - minlen + 1; i++)
		if (brute_force_fixed_length(hash, result, str, 0, minlen-1+i))
			return true;

	delete[] str;

	return false;
}
