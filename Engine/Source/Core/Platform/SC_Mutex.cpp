
#include "SC_Mutex.h"

SC_Mutex::SC_Mutex(int aSpinLockCount /*= 0*/)
	: mInternalMutex(aSpinLockCount)
{
	mLockCount = 0;
}

void SC_Mutex::Lock()
{
	if (!mInternalMutex.TryLock())
	{
		// Track locks?
		mInternalMutex.Lock();
	}

	++mLockCount;
}

bool SC_Mutex::TryLock()
{
	if (!mInternalMutex.TryLock())
		return false;

	++mLockCount;
	return true;
}

void SC_Mutex::Unlock()
{
	assert((mLockCount > 0));
	--mLockCount;

	mInternalMutex.Unlock();
}

uint32 SC_Mutex::GetLockedCount()
{
	return mLockCount;
}
