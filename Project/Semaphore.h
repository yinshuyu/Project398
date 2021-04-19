/*Start Header
******************************************************************/
/*!
\file Semaphore.h
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
#pragma once
#include <mutex>
#include <condition_variable>

class Semaphore
{
private:
	int sem;
	std::condition_variable cv;
	std::mutex mtx;
public:

	Semaphore(int i = 0) :sem(i) {}

	void wait()
	{
		std::unique_lock<std::mutex> lk(mtx);
		while (sem == 0)
		{
			/*
			condition variable wait performs
			1. unlocks the mtx
			2. wait for notification i.e., sleep
			3. locks the mtx back after waking up


			*/
			cv.wait(lk);
		}
		sem--;
	}

	void signal()
	{
		std::lock_guard<std::mutex> lk(mtx);
		sem++;
		cv.notify_one();
	}
};