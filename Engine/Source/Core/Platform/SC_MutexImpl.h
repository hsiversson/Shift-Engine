#pragma once

class SC_MutexImpl
{
	friend class SC_Mutex;
protected:
	SC_MutexImpl(uint32 aSpinLockCount);
	~SC_MutexImpl();

	void Lock();
	bool TryLock();
	void Unlock();

#if IS_WINDOWS_PLATFORM
	CRITICAL_SECTION mCriticalSection;
#else
#error Platform not supported!
#endif
};
