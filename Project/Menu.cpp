/*Start Header
******************************************************************/
/*!
\file Menu.cpp
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

#include "Menu.h"
#include "md5_crack.h"
#include "clearscreenw.h"


//while loop - start up interface
void Project::MainMenu()
{
	while (1)
	{
		ClearScreen();
		std::cout << "CS398 Project - MD5 Hash Cracking Tool \n";
		std::cout << std::endl;
		std::cout << "1) Generate MD5 Hash" << std::endl;
		std::cout << "2) Crack MD5 Hash " << std::endl;
		std::cout << std::endl;
		std::cout << "3) Exit" << std::endl;
		std::cout << std::endl;
		std::cout << "Choose your operation:" << std::endl;

		std::string choice = "";
		std::cin >> choice;
		std::cout << std::endl;

		{
			switch (choice.at(0)) {
			case '1':
				MD5_Generating();  //change to generate md5 hash interface
				break;
			case '2':
				CrackMenu();  //change to cracking md5 hash interface
				continue;
			case '3':
				std::cout << "Exiting Program...." << std::endl;
				pause();
				return;
			default:
				std::cout << "Invalid Choice...." << std::endl;
			}

			pause();
		}
	}
}

void Project::pause()
{
	std::cout << std::endl;
	std::cout << "Press Any Key to Continue...";

	std::cin.get();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear stream
}


//change to generate md5 hash interface
void Project::MD5_Generating()
{
	ClearScreen();
	std::cout << "Operation - Generate MD5 Hash \n";
	std::cout << std::endl;
	std::cout << "Enter your message to generate MD5 Hash: " << std::endl;

	std::string message = "";
	std::cin >> message;

	char str[MD5_STRING_SIZE];

	{
		md5::md5_t md5;

		md5.process(message.c_str(), message.length());
		md5.finish();
		md5.get_string(str);
	}
	std::cout << std::endl;
	std::cout << "MD5 Hash generated from message (" << message << ") :" << std::endl;
	//for (unsigned int i = 0; i < MD5_STRING_SIZE; i++)
	//	std::cout << str[i];

	std::cout << str << std::endl;
	std::cout << std::endl;
}

//change to cracking md5 hash interface
void Project::CrackMenu()
{
	bool dLoad = false, dAttack = false;

	std::string cMethod = "1";

	bool hLoad = false;
	std::string hHash = "0";

	while (1)
	{
		ClearScreen();

		//interface words
		{
			std::cout << "Operation - Crack MD5 Hash \n";
			std::cout << std::endl;
			std::cout << "1) Crack Hash" << std::endl;
			std::cout << std::endl;
			std::cout << "2) Load Hash to be Cracked - Currently: [";
			{
				if (hLoad)
				{
					std::cout << "Loaded]" << std::endl;
					std::cout << "-> Hash Loaded: [" << hHash << "]" << std::endl;
				}
				else
					std::cout << "Unloaded]" << std::endl;
			}
			if (dAttack)
			{
				std::cout << "3) Link Dictionary - Currently: [";
				if (dLoad)
				{
					std::cout << "Loaded]" << std::endl;
          std::cout << "-> Dictionary Loaded: [" << _dictionaryLocation << "]" << std::endl;
          std::cout << "-> " << _dictionarySize << " Messages in Dictionary file!" << std::endl;
          std::cout << "-> Maximum Message length: " << _dictionaryMaxMsgLength << std::endl;
				}
				else
					std::cout << "Unloaded]" << std::endl;
			}
			std::cout << std::endl;
			std::cout << "4) Set Cracking method - Currently: [";
			{
				switch (cMethod.at(0)) {
				case '1':
					std::cout << "Brute Force Attack]" << std::endl;
					break;
				case '2':
					std::cout << "Dictionary Attack]" << std::endl;
					break;
				case '3':
					std::cout << "Brute Force + Dictionary attack]" << std::endl;
					break;
				default:
					break;
				}
			}
			if (cMethod.at(0) != '2')
      {
        std::cout << "5) Set Brute Force Message Length - Currently: [" << _messageLength.first;
        std::cout << " ~ " << _messageLength.second << "]" << std::endl;
      }
      std::cout << std::endl;
      std::cout << "6) Back to Menu" << std::endl;
      std::cout << std::endl;
      std::cout << "Choose your operation:" << std::endl;
		}

		std::string choice = "";
		std::cin >> choice;
		std::cout << std::endl;

		{
			switch (choice.at(0)) {
			case '4':
				{
					SetMethod(cMethod, dAttack);
					dLoad = false;
					hLoad = false;
					_messageLength.first = 1;
					_messageLength.second = 3;
				}
				break;
			case '2':
				hLoad = Loadhash(hHash);
				break;
			case '1':
				if (hLoad)
				{
					if(dAttack)
					{ 
						if(dLoad)
							MD5_Cracking(hHash, cMethod);
						else
							std::cout << "Your Dictionary is not loaded!" << std::endl;
					}
					else
						MD5_Cracking(hHash, cMethod);
				}
				else
					std::cout << "Your MD5 Hash is not loaded!" << std::endl;
				break;
			case '6':
				std::cout << "Returning to Menu...." << std::endl;
				pause();
				return;
			case '5':
				if (cMethod.at(0) != '2') 
					MessageSetting();
				else
					std::cout << "Invalid Choice...." << std::endl;
				break;
			case '3':
				if (dAttack)
				{
					dLoad = LoadDictionary();
					break;
				}
			default:
				std::cout << "Invalid Choice...." << std::endl;
			}
			pause();
		}
	}
}

void Project::MD5_Cracking(std::string hash, std::string cMethod)
{
	ClearScreen();
	std::cout << "Operation - Crack Hash \n";
	std::cout << std::endl;

	switch (cMethod.at(0)) {
	case '1':
		BruteForceAttack(_argc, _argv, hash.c_str(), _messageLength.first, _messageLength.second);
		break;
	case '2':
		DictionaryAttack(_argc, _argv, hash.c_str(), _dictionaryLocation, _dictionarySize, _dictionaryMaxMsgLength);
		break;
	case '3':
		DictionaryAttack(_argc, _argv, hash.c_str(), _dictionaryLocation, _dictionarySize, _dictionaryMaxMsgLength);
		BruteForceAttack(_argc, _argv, hash.c_str(), _messageLength.first, _messageLength.second);
		break;
	default:
		break;
	}
	
}

bool Project::Loadhash(std::string& hash)
{
	ClearScreen();
	std::cout << "Operation - Load Hash to be Cracked \n";
	std::cout << std::endl;
	std::cout << "Enter your MD5 Hash: " << std::endl;

	std::string inputHash = "";
	std::cin >> inputHash;
	std::cout << std::endl;

	if (inputHash.length() == 32)
	{
		std::cout << "Hash Successfully Loaded!" << std::endl;
		hash = inputHash;
		return true;
	}
	else 
		std::cout << "Hash entered was not valid!" << std::endl;

	return false;
}

void Project::SetMethod(std::string& method, bool& dAttack)
{
	ClearScreen();
	std::cout << "Operation - Set Cracking method \n";
	std::cout << std::endl;
	std::cout << "1) Brute Force Attack" << std::endl;
	std::cout << "2) Dictionary Attack" << std::endl;
	std::cout << "3) Brute Force + Dictionary attack" << std::endl;
	std::cout << std::endl;
	std::cout << "Choose your MD5 Hash Method: " << std::endl;

	std::string choice = "";
	std::cin >> choice;
	std::cout << std::endl;

	{
		switch (choice.at(0)) {
		case '1':
			std::cout << "Successful Chosen Brute Force Attack!" << std::endl;
			method = choice;
			dAttack = false;
			break;
		case '2':
			std::cout << "Successful Chosen Dictionary Attack!" << std::endl;
			method = choice;
			dAttack = true;
			break;
		case '3':
			std::cout << "Successful Chosen Brute Force + Dictionary attack!" << std::endl;
			method = choice;
			dAttack = true;
			break;
		default:
			std::cout << "Invalid Choice...." << std::endl;
		}
	}
}

bool Project::LoadDictionary()
{
	ClearScreen();
	std::cout << "Operation - Link Dictionary \n";
	std::cout << std::endl;
	std::cout << "Enter Dictionary Location (E.g. pass.txt):" << std::endl;

	std::string filename = "";
	std::cin >> filename;
	std::cout << std::endl;

	if (!LoadFile(filename, _dictionarySize, _dictionaryMaxMsgLength))
	{
		std::cout << "Dictionary file does not exist!" << std::endl;
		return false;
	}

	_dictionaryLocation = filename;

	std::cout << "Dictionary file Connection is linked!" << std::endl;
	std::cout << _dictionarySize << " Messages in Dictionary file!" << std::endl;
	std::cout  << "Maximum Message length: " << _dictionaryMaxMsgLength << std::endl;

	return true;
}

void Project::MessageSetting()
{
	ClearScreen();
	std::cout << "Operation - Set Brute Force Message Length \n";
	unsigned length = 0;

	while (1)
	{
		std::cout << std::endl;
		std::cout << "Enter Message Minimum Length :" << std::endl;
		std::cin >> length;

		if (length < 1)
		{
			std::cout << "Error: Minimum Length cannot be less than 1!" << std::endl;
			continue;
		}

		_messageLength.first = length;

		std::cout << std::endl;
		std::cout << "Enter Message Maximum Length :" << std::endl;
		std::cin >> length;

		if (_messageLength.first > length)
			std::cout << "Error: Maximum Length is smaller than Minimum Length!" << std::endl;
		else
			break;
	}
	_messageLength.second = length;

	std::cout << std::endl;
	std::cout << "Operation Done! \n";
	std::cout << "Message Minimum Length :" << _messageLength.first << std::endl;
	std::cout << "Message Maximum Length :" << _messageLength.second << std::endl;
}