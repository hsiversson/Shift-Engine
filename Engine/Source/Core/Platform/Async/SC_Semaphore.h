#pragma once

#if IS_WINDOWS_PLATFORM
#include "SC_Semaphore_Win64.h"
#else
#error Platform Not supported yet!
#endif

class SC_Semaphore
{
public:
	SC_Semaphore(uint32 aCount = 0);
	~SC_Semaphore();

	void Release(uint32 aCount = 1);

	bool TimedAcquire(uint32 aMilliseconds);
	void Acquire();

private:
	SC_SemaphoreImpl mSemaphoreImpl;
};

