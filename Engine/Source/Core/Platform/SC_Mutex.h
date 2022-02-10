#pragma once
#include "SC_MutexImpl.h"
#include "SC_AutoLock.h"

class SC_Mutex
{
public:
	SC_Mutex(int aSpinLockCount = 0);
	SC_Mutex(const SC_Mutex&) = delete;
	SC_Mutex& operator=(const SC_Mutex&) = delete;

	void Lock();
	bool TryLock();
	void Unlock();

	uint32 GetLockedCount();

private:
	SC_MutexImpl mInternalMutex;
	uint32 mLockCount;
};

using SC_MutexLock = SC_AutoLock<SC_Mutex>;

