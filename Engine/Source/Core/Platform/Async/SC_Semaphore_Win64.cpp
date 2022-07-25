#include "SC_Semaphore_Win64.h"

#if IS_WINDOWS_PLATFORM

SC_Semaphore_Win64::SC_Semaphore_Win64(uint32 aCount /*= 0*/)
{
	mSemaphoreHandle = ::CreateSemaphoreEx(nullptr, aCount, INT_MAX, nullptr, 0, SEMAPHORE_ALL_ACCESS);
	SC_ASSERT(mSemaphoreHandle != nullptr);
}

SC_Semaphore_Win64::~SC_Semaphore_Win64()
{
	CloseHandle(mSemaphoreHandle);
}

bool SC_Semaphore_Win64::Acquire()
{
	bool result = (WaitForSingleObjectEx(mSemaphoreHandle, INFINITE, false) != WAIT_FAILED);
	SC_ASSERT(result);
	return result;
}

bool SC_Semaphore_Win64::TimedAcquire(uint32 aTimeoutPeriod)
{
	bool waitedFor = false;

	DWORD result = WaitForSingleObjectEx(mSemaphoreHandle, aTimeoutPeriod, false);
	if (result == WAIT_OBJECT_0)
		waitedFor = true;
	else if (result == WAIT_TIMEOUT)
		waitedFor = false;
	else
		SC_ASSERT(false, "Failed to wait for semaphore.");

	return waitedFor;
}

void SC_Semaphore_Win64::Release(uint32 aCount /*= 1*/)
{
	ReleaseSemaphore(mSemaphoreHandle, aCount, nullptr);
}

#endif