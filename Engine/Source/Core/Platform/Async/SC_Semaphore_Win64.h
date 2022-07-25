#pragma once

#if IS_WINDOWS_PLATFORM
class SC_Semaphore_Win64
{
public:
	SC_Semaphore_Win64(uint32 aCount = 0);
	~SC_Semaphore_Win64();

	bool Acquire();
	bool TimedAcquire(uint32 aTimeoutPeriod);

	void Release(uint32 aCount = 1);

private:
	HANDLE mSemaphoreHandle;
};

using SC_SemaphoreImpl = SC_Semaphore_Win64;
#endif

