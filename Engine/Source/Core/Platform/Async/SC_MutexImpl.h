#pragma once

#if IS_WINDOWS_PLATFORM
class SC_MutexImpl_Win64
{
	friend class SC_Mutex;
protected:
	SC_MutexImpl_Win64();
	~SC_MutexImpl_Win64();

	void Lock();
	bool TryLock();
	void Unlock();

	CRITICAL_SECTION mCriticalSection;
};

class SC_ReadWriteMutexImpl_Win64
{
public:
	SC_ReadWriteMutexImpl_Win64();

	bool TryBeginRead();
	void BeginRead();
	void EndRead();

	bool TryBeginWrite();
	void BeginWrite();
	void EndWrite();

private:
	SRWLOCK mSRWLock;
};

using SC_MutexImpl = SC_MutexImpl_Win64;
using SC_ReadWriteMutexImpl = SC_ReadWriteMutexImpl_Win64;

#else
#error Platform not supported!
#endif