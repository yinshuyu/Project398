/*Start Header
******************************************************************/
/*!
\file main.cpp
\author	Yin shuyu, yin.s, 1802075
\co-author Luo Yu Xuan, yuxuan.luo, 1802205
\par yin.s\@digipen.edu
\co-par yuxuan.luo\@digipen.edu
\date Apr 19, 2021
\brief  CS398 Final Project
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "menu.h"

int main(int argc, char **argv)
{
	Project MD5_Tool(argc, argv); //make a project

	MD5_Tool.MainMenu(); //Start Main Menu of Project 

	return 0;
}