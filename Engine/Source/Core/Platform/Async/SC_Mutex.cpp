#include "SC_Mutex.h"

SC_Mutex::SC_Mutex()
{
	mLockCount = 0;
}

void SC_Mutex::Lock() const
{
	if (!mInternalMutex.TryLock())
	{
		// Track locks?
		mInternalMutex.Lock();
	}

	++mLockCount;
}

bool SC_Mutex::TryLock() const
{
	if (!mInternalMutex.TryLock())
		return false;

	++mLockCount;
	return true;
}

void SC_Mutex::Unlock() const
{
	assert((mLockCount > 0));
	--mLockCount;

	mInternalMutex.Unlock();
}

uint32 SC_Mutex::GetLockedCount() const
{
	return mLockCount;
}

SC_ReadWriteMutex::SC_ReadWriteMutex()
{

}

bool SC_ReadWriteMutex::TryBeginRead() const
{
	return mInternalRWMutex.TryBeginRead();
}

void SC_ReadWriteMutex::BeginRead() const
{
	if (TryBeginRead())
		return;

	mInternalRWMutex.BeginRead();
}

void SC_ReadWriteMutex::EndRead() const
{
	mInternalRWMutex.EndRead();
}

bool SC_ReadWriteMutex::TryBeginWrite() const
{
	return mInternalRWMutex.TryBeginWrite();
}

void SC_ReadWriteMutex::BeginWrite() const
{
	if (TryBeginWrite())
		return;

	mInternalRWMutex.BeginWrite();
}

void SC_ReadWriteMutex::EndWrite() const
{
	mInternalRWMutex.EndWrite();
}
