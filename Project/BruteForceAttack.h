#pragma once
#include <cstdint>
#include "md5.h"

#define START_0   0x30  // 0
#define END_9     0x39  // 9

#define START_A   0x41  // A
#define END_Z     0x5A  // Z

#define START_a   0x61  // a
#define END_z     0x7A  // z

#define START   0x20  // space
#define END     0x7E  // ~

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

bool brute_force(const char* hash, std::string& result, unsigned minlen, unsigned maxlen) {

	char* str = new char[maxlen+1];

	for (unsigned i = 0; i < maxlen+1; i++)
		str[i] = '\0';

	for (unsigned i = 0; i < maxlen - minlen + 1; i++)
		if (brute_force_fixed_length(hash, result, str, 0, minlen-1+i))
			return true;

	delete[] str;

	return false;
}
