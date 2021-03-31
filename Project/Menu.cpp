#include "Menu.h"
#include "BruteForceAttack.h"
#include "clearscreenw.h"

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
				MD5_Generating();
				break;
			case '2':
				CrackMenu();
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
	for (unsigned int i = 0; i < MD5_STRING_SIZE; i++)
		std::cout << str[i];
	std::cout << std::endl;

	//// more Operation



}

void Project::CrackMenu()
{
	bool dLoad = false, dAttack = false;

	std::string cMethod = "1";

	bool hLoad = false;
	std::string hHash = "0";

	while (1)
	{
		ClearScreen();
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
		std::cout << "5) Message Setting" << std::endl;
		if(dAttack && dLoad)
			std::cout << "6) Dictionary Setting" << std::endl;
		std::cout << std::endl;
		std::cout << "7) Back to Menu" << std::endl;
		std::cout << std::endl;
		std::cout << "Choose your operation:" << std::endl;

		std::string choice = "";
		std::cin >> choice;
		std::cout << std::endl;

		{
			switch (choice.at(0)) {
			case '4':
				SetMethod(cMethod, dAttack);
				break;
			case '2':
				hLoad = Loadhash(hHash);
				break;
			case '1':
				if (hLoad )
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
			case '7':
				std::cout << "Returning to Menu...." << std::endl;
				pause();
				return;
			case '5':
				MessageSetting();
				break;
			case '6':
				if (dAttack && dLoad)
				{
					DictionarySetting();
					break;
				}
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

	std::string message = "";
	std::unordered_map<std::string, std::string>::iterator it;

	switch (cMethod.at(0)) {
	case '1':
		std::cout << "Brute Force Attacking...." << std::endl;
		std::cout << std::endl;

		if (brute_force(hash.c_str(), message, 10))
		{
			std::cout << "Hash (" << hash;
			std::cout << ") Cracked: Message (" << message  << ")!" << std::endl;
		}
		else
		{
			std::cout << "Failed to crack Hash (" << hash;
			std::cout <<  ")!" << std::endl;
		}
		break;
	case '2':
		std::cout << "Dictionary Attacking...." << std::endl;
		std::cout << std::endl;

		it = _dictionary.find(hash);
		if (it != _dictionary.end())
		{
			std::cout << "Hash (" << hash;
			std::cout << ") Cracked: Message (" << it->second << ")!" << std::endl;
		}
		else
		{
			std::cout << "Failed to crack Hash (" << hash;
			std::cout << ")!" << std::endl;
		}
		break;
	case '3':
		std::cout << "Brute Force + Dictionary attacking...." << std::endl;
		std::cout << std::endl;

		std::cout << "Dictionary Attacking...." << std::endl;

		it = _dictionary.find(hash);
		if (it == _dictionary.end())
		{
			std::cout << "....No Matching Hash found in Dictionary" << std::endl;
			std::cout << std::endl;

			std::cout << "Brute Force Attacking...." << std::endl;

			if (!brute_force(hash.c_str(), message, 10))
			{
				std::cout << "Failed to crack Hash (" << hash;
				std::cout << ")!" << std::endl;
				break;
			}
		}
		else
			message = it->second;

		std::cout << "Hash (" << hash;
		std::cout << ") Cracked: Message (" << message << ")!" << std::endl;

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

	std::ifstream f;
	f.open(filename);

	if (!f) {
		std::cout << "Dictionary file does not exist!" << std::endl;
		return false;
	}
	else {

		_dictionaryLocation = filename;

		_dictionary.clear();
		std::cout << "Extracting Dictionary Data..." << std::endl;
		std::string line;
		std::string delimiter = ";";


		for (int i = 0; std::getline(f, line); ++i) 
		{ 
			size_t pos = line.find(delimiter);
			if (pos != std::string::npos)
			{
				std::string hash = line.substr(0, pos);
				line.erase(0, pos + delimiter.length());
				_dictionary.insert({ hash , line });
			}
		};

		f.close();

		std::cout << std::endl;
		std::cout << "Extraction Completed! " << std::endl; 
		std::cout << _dictionary.size() << " sets of MD5 Hash Message loaded in!" << std::endl;

		return true;
	}
}

void Project::MessageSetting()
{
	ClearScreen();
	std::cout << "Operation - Message Setting \n";
	std::cout << std::endl;

	std::cout << "Not done yet\n Need to set the Brute Force message search Length\n And the type of character it has in the message" << std::endl;

}

void Project::DictionarySetting()
{
	while (1)
	{
		ClearScreen();
		std::cout << "Operation - Dictionary Setting \n";
		std::cout << std::endl;

		std::cout << "1) Show All Message MD5 Hash in Dictionary" << std::endl;
		std::cout << "2) Add New Message MD5 Hash into Dictionary" << std::endl;
		std::cout << "3) Remove Exsiting Message MD5 Hash from Dictionary" << std::endl;
		std::cout << "4) Clean All Dictionary" << std::endl;
		std::cout << "5) Update FileData" << std::endl;
		std::cout << std::endl;
		std::cout << "6) Return to Crack MD5 Hash Menu" << std::endl;
		std::cout << std::endl;
		std::cout << "Choose your operation:" << std::endl;

		std::string choice = "";
		std::cin >> choice;
		std::cout << std::endl;

		{
			switch (choice.at(0)) {
			case '1':
				PrintAllDictionary();
				break;
			case '2':
				AddNewMD5MessageHash();
				break;
			case '3':
				RemoveMD5MessageHash();
				break;
			case '4':
				ClearAllDictionary();
				break;
			case '5':
				UpdateFileData();
				break;
			case '6':
				std::cout << "Returning to Crack MD5 Hash Menu...." << std::endl;
				return;
			default:
				std::cout << "Invalid Choice...." << std::endl;
			}

			pause();
		}
	}
}

void Project::PrintAllDictionary()
{
	ClearScreen();
	std::cout << "Operation - Show All Message MD5 Hash in Dictionary \n";
	std::cout << std::endl;

	for (const auto& m : _dictionary) {
		std::cout << "Hash: ["<< m.first << "] ==> Message: [" << m.second << "]" << std::endl;
	}
}

void Project::AddNewMD5MessageHash()
{
	ClearScreen();
	std::cout << "Operation - Add New Message MD5 Hash into Dictionary \n";
	std::cout << std::endl;
	std::cout << "Enter your message to be MD5 Hash into Dictionary: " << std::endl;

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

	std::string hash{ str };

	std::ofstream ansf;
	ansf.open(_dictionaryLocation, std::ios::app);
	ansf << hash<<";"<< message << std::endl; //Write the password at the eof


	_dictionary.insert({ hash , message });

	std::cout << "Hash: [" << hash << "] ==> Message: [" << message << "]" << std::endl;
	std::cout << "Added into Dictionary" << std::endl;

}

void Project::RemoveMD5MessageHash()
{
	ClearScreen();
	std::cout << "Operation - Remove Exsiting Message MD5 Hash from Dictionary \n";
	std::cout << std::endl;
	std::cout << "Enter your message that MD5 Hash will be remove from Dictionary: " << std::endl;

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

	std::string hash{ str };

	std::unordered_map<std::string, std::string>::iterator it;
	it = _dictionary.find(hash);
	if (it == _dictionary.end())
	{
		std::cout << "Message dont Exist in Dictionary!" << std::endl;
		return;
	}
	else
	{
		_dictionary.erase(it);

		std::cout << "Hash: [" << hash << "] ==> Message: [" << message << "]" << std::endl;
		std::cout << "Removed from Dictionary" << std::endl;
	}
}

void Project::ClearAllDictionary()
{
	ClearScreen();
	std::cout << "Operation - Clean All Dictionary \n";
	std::cout << std::endl;

	_dictionary.clear();

	std::cout << "Dictionary Size: " << _dictionary.size();
}

void Project::UpdateFileData()
{
	ClearScreen();
	std::cout << "Operation - Update FileData \n";
	std::cout << std::endl;

	std::fstream ofs;
	ofs.open(_dictionaryLocation, std::ios::out | std::ios::trunc);

	for (const auto& m : _dictionary) {
		ofs << m.first << ";" << m.second << std::endl;
	}

	ofs.close();

	std::cout << "Update Done!" << std::endl;
}

