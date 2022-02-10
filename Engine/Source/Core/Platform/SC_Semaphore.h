#pragma once
#include <mutex>

class SC_Semaphore
{
public:
	SC_Semaphore(uint32 aCount = 0);
	~SC_Semaphore();

	void Signal(uint32 aCount = 1);

	bool WaitFor(uint32 aMilliseconds);
	void Wait();

private:
	uint32 mCount;
	std::mutex mMutex;
	std::condition_variable mCondition;
};

