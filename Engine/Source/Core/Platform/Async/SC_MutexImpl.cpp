
#include "SC_MutexImpl.h"

#if IS_WINDOWS_PLATFORM
#include <synchapi.h>

SC_MutexImpl_Win64::SC_MutexImpl_Win64()
{
	const uint32 spinLockCount = 0;
	InitializeCriticalSectionEx(&mCriticalSection, spinLockCount, 0);
}

SC_MutexImpl_Win64::~SC_MutexImpl_Win64()
{
	DeleteCriticalSection(&mCriticalSection);
}

void SC_MutexImpl_Win64::Lock()
{
	EnterCriticalSection(&mCriticalSection);
}

bool SC_MutexImpl_Win64::TryLock()
{
	return (TryEnterCriticalSection(&mCriticalSection) != 0);
}

void SC_MutexImpl_Win64::Unlock()
{
	LeaveCriticalSection(&mCriticalSection);
}

SC_ReadWriteMutexImpl_Win64::SC_ReadWriteMutexImpl_Win64()
{
	InitializeSRWLock(&mSRWLock);
}

bool SC_ReadWriteMutexImpl_Win64::TryBeginRead()
{
	return TryAcquireSRWLockShared(&mSRWLock) != 0;
}

void SC_ReadWriteMutexImpl_Win64::BeginRead()
{
	AcquireSRWLockShared(&mSRWLock);
}

void SC_ReadWriteMutexImpl_Win64::EndRead()
{
	ReleaseSRWLockShared(&mSRWLock);
}

bool SC_ReadWriteMutexImpl_Win64::TryBeginWrite()
{
	return TryAcquireSRWLockExclusive(&mSRWLock) != 0;
}

void SC_ReadWriteMutexImpl_Win64::BeginWrite()
{
	AcquireSRWLockExclusive(&mSRWLock);
}

void SC_ReadWriteMutexImpl_Win64::EndWrite()
{
	ReleaseSRWLockExclusive(&mSRWLock);
}

#else
#error Platform not supported!
#endif