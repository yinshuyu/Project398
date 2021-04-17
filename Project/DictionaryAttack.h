#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "md5.h"

bool LoadFile(std::string filename, unsigned& size, unsigned& length)
{
	std::ifstream f;
	f.open(filename);

	if (!f)  return false;

	std::string line;
	unsigned i;
	unsigned maxLength;

	for (i = 0, maxLength = 0; std::getline(f, line); ++i)
	{
		if (maxLength < line.length())
			maxLength = line.length();
	}

	size = i;
	length = maxLength;

	return true;
}

void LoadDictionaryMemory(char*& list, unsigned listSize, unsigned messageLength)
{
	list = new char[listSize * messageLength];
}

void FreeDictionaryMemory(char*& list)
{
	delete[] list;
}

bool LoadDictionary(std::string filename, char*& list, unsigned msgMaxLgth)
{
	std::ifstream f;
	f.open(filename);

	if (!f)
		return false;

	std::string line;

	for (unsigned i = 0; std::getline(f, line); ++i)
	{
		unsigned n = 0;
		for (; n < line.length(); n++) {
			list[i * msgMaxLgth + n] = line.at(n);
		}
		for (; n < msgMaxLgth; n++) {
			list[i * msgMaxLgth + n] = '\0';
		}
	};

	f.close();

	return true;
}


bool ScanDictionary(const char* hash, std::string& result, char*& list, unsigned listSize, unsigned msgMaxLgth)
{
	static char chash[MD5_STRING_SIZE];

	for (unsigned i = 0; i < listSize; i++)
	{
		unsigned length = 0;

		while (list[i * msgMaxLgth + length] != '\0')
			length++;

		md5::md5_t md5;
		md5.process(&(list[i * msgMaxLgth]), length);
		md5.finish();

		md5.get_string(chash);

		std::string s(chash);

		if (!s.compare(hash))
		{
			result = &(list[i * msgMaxLgth]);
			return true;
		}

	}

	return false;
}