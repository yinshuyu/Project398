/*Start Header
******************************************************************/
/*!
\file Menu.h
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

	void ModeSetting();
	void BlockSizeSetting();
	void TileSizeSetting();
	void StreamSizeSetting();



	int _argc;
	char** _argv;
	std::string _dictionaryLocation; //dictionary file name
	unsigned _dictionarySize;
	unsigned _dictionaryMaxMsgLength; // maximum message length in dictionary 
	std::pair<unsigned, unsigned> _messageLength; //minimum length ~ maximum length for Brute Force
	unsigned _blockSize;
	unsigned _tileSize;
	unsigned _mode;
	unsigned numberOfStreams{3};

public:

	Project(int argc, char** argv) :
		_argc(argc),
		_argv(argv),
		_dictionaryLocation(),
		_dictionarySize(0),
		_messageLength({ 1,3 }),
		_blockSize{32},
		_tileSize{512},
		_mode{0}
	{}

	~Project() {}

	void MainMenu();
};