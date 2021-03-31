#pragma once
#include <cstdint>
#include "md5.h"

#define START   0x61  // a
#define END     0x7A  // z

typedef unsigned char byte_t;

int compare(const uint32_t a[], const uint32_t b[]) {
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
}

int brute_force_fixed_length(const char* hash, std::string& result, char* test, int p, int len) {

	static char chash[MD5_STRING_SIZE];

	char c;

	if (p < len) {
		for (c = START; c <= END; ++c) {
			test[p] = c;
			if (brute_force_fixed_length(hash, result, test, p + 1, len))
				return 1;
		}
	}
	else {
		for (c = START; c <= END; ++c) {
			test[p] = c;

			md5::md5_t md5;
			md5.process(test, len + 1);
			md5.finish();

			md5.get_string(chash);

			std::string s(chash);

			if (!s.compare(hash))
			{
				result = test;
				return 1;
			}
		}
	}

	return 0;
}

int brute_force(const char* hash, std::string& result, int maxlen) {

	char* str = new char[maxlen+1];

	for (int i = 0; i < maxlen+1; i++)
		str[i] = '\0';

	for (int i = 0; i < maxlen; i++)
		if (brute_force_fixed_length(hash, result, str, 0, i)) 
			return 1;

	delete[] str;

	return 0;
}
