#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <fstream>

class Project
{
	void pause();

	void MD5_Generating();

	void CrackMenu();
	void MD5_Cracking(std::string hash, std::string cMethod);


	bool Loadhash(std::string& hash);
	void SetMethod(std::string& method, bool& dAttack);

	bool LoadDictionary();

	void MessageSetting();

	int _argc;
	char** _argv;
	std::string _dictionaryLocation; //dictionary file name
	unsigned _dictionarySize;
	unsigned _dictionaryMaxMsgLength; // maximum message length in dictionary 
	std::pair<unsigned, unsigned> _messageLength; //minimum length ~ maximum length for Brute Force

public:

	Project(int argc, char** argv) :
		_argc(argc),
		_argv(argv),
		_dictionaryLocation(),
		_dictionarySize(0),
		_messageLength({ 1,3 })
	{}

	~Project() {}

	void MainMenu();
};