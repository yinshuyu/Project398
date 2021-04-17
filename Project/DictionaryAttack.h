#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "md5.h"

//load file - finds out the size of the dictionary and the maximum message length in dictionary
bool LoadFile(std::string filename, unsigned& size, unsigned& length)
{
	std::ifstream f;
	f.open(filename); //open file

	if (!f)  return false;  //if file doesnt exist, return false

	std::string line;
	unsigned i;
	unsigned maxLength;

	for (i = 0, maxLength = 0; std::getline(f, line); ++i) 
	{
    //find out the size of the dictionary 
    //- how many passwords are there in the dictionary
    //- the maximum message length in dictionary
		if (maxLength < line.length())
			maxLength = line.length();
	}

	size = i;
	length = maxLength; 

	return true;  //file exists
}

//allocate memory for the entire dictionary list
void LoadDictionaryMemory(char*& list, unsigned listSize, unsigned messageLength)
{
	list = new char[listSize * messageLength];
}

//Free the entire dictionary list
void FreeDictionaryMemory(char*& list)
{
	delete[] list;
}

//load in all passwords of the dictionary 
//into a 1D array - list - dynamically allocated with the msgMaxLength for every password
bool LoadDictionary(std::string filename, char*& list, unsigned msgMaxLgth)
{
	std::ifstream f;
	f.open(filename);

	if (!f)
		return false;

	std::string line; 

	//go through every line of the dictionary
	for (unsigned i = 0; std::getline(f, line); ++i)
	{
		unsigned n = 0;
		//convert string to char* 
		for (; n < line.length(); n++) {

			list[i * msgMaxLgth + n] = line.at(n);
		}

		//max length - 6
		//msg - hi
		//hi\0\0\0\0
		for (; n < msgMaxLgth; n++) {
			list[i * msgMaxLgth + n] = '\0';
		}
	};

	f.close();

	return true;
}


//Cracking the User MD5 hash string using the Dictionary attack method

//hash - user input of a MD5 hash string
bool ScanDictionary(const char* hash, std::string& result, 
	char*& list, unsigned listSize, unsigned msgMaxLgth)
{
	static char chash[MD5_STRING_SIZE];

	for (unsigned i = 0; i < listSize; i++)
	{
		unsigned length = 0;

		while (list[i * msgMaxLgth + length] != '\0')
			length++;


		//hash a single dictionary password to MD5hash format
		md5::md5_t md5;
		md5.process(&(list[i * msgMaxLgth]), length);
		md5.finish();

		md5.get_string(chash);

		std::string s(chash);

		//compare if the dictionary MD5hash format is the same as the user input
		if (!s.compare(hash))
		{
			//convert the dictionary char * to std::string
			result = &(list[i * msgMaxLgth]);
			//found the password in the dictionary
			return true;
		}

	}

	//did not find the password in the dictionary
	return false;
}