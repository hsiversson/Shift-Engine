#pragma once
#include "SC_MutexImpl.h"
#include "SC_AutoLock.h"

class SC_Mutex
{
public:
	SC_Mutex();
	SC_Mutex(const SC_Mutex&) = delete;
	SC_Mutex& operator=(const SC_Mutex&) = delete;

	void Lock() const;
	bool TryLock() const;
	void Unlock() const;

	uint32 GetLockedCount() const;

private:
	mutable SC_MutexImpl mInternalMutex;
	mutable uint32 mLockCount;
};

using SC_MutexLock = SC_AutoLock<SC_Mutex>;


class SC_ReadWriteMutex
{
public:
	SC_ReadWriteMutex();

	bool TryBeginRead() const;
	void BeginRead() const;
	void EndRead() const;

	bool TryBeginWrite() const;
	void BeginWrite() const;
	void EndWrite() const;

private:
	mutable SC_ReadWriteMutexImpl mInternalRWMutex;
};
