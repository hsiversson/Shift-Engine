
#include "SC_MutexImpl.h"

#if IS_WINDOWS_PLATFORM
#include <synchapi.h>
#endif

SC_MutexImpl::SC_MutexImpl(uint32 aSpinLockCount)
{
#if IS_WINDOWS_PLATFORM
	InitializeCriticalSectionEx(&mCriticalSection, (DWORD)aSpinLockCount, 0);
#else
#error Platform not supported!
#endif
}

SC_MutexImpl::~SC_MutexImpl()
{
#if IS_WINDOWS_PLATFORM
	DeleteCriticalSection(&mCriticalSection);
#else
#error Platform not supported!
#endif
}

void SC_MutexImpl::Lock()
{
#if IS_WINDOWS_PLATFORM
	EnterCriticalSection(&mCriticalSection);
#else
#error Platform not supported!
#endif
}

bool SC_MutexImpl::TryLock()
{
#if IS_WINDOWS_PLATFORM
	return (TryEnterCriticalSection(&mCriticalSection) != 0);
#else
#error Platform not supported!
#endif
}

void SC_MutexImpl::Unlock()
{
#if IS_WINDOWS_PLATFORM
	LeaveCriticalSection(&mCriticalSection);
#else
#error Platform not supported!
#endif
}