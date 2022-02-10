#pragma once
#include "SC_Semaphore.h"

#ifdef Yield
#undef Yield
#endif

#if IS_WINDOWS_PLATFORM
using SC_ThreadHandle = HANDLE;
using SC_ThreadId = DWORD;
#else
#error Platform not supported!
#endif

class SC_Thread
{
public:
	SC_Thread();

	SC_Thread(const SC_Thread&) = delete;
	void operator=(const SC_Thread&) = delete;
	virtual ~SC_Thread();

	void Start();
	virtual void Stop(bool aWaitForFinish = false);
	void Wait();
	bool IsActive() const;

	SC_Semaphore& GetSemaphore();

	SC_ThreadHandle GetHandle() const;
	SC_ThreadId GetId() const;

	void SetName(const char* aName);

	static void Yield();
	static void Sleep(uint32 aMilliseconds);

	static SC_Thread* GetCurrentThread();
	static SC_ThreadId GetCurrentThreadId();
	static SC_ThreadId GetMainThreadId();

protected:
	virtual void ThreadMain();

private:
	static uint32 __stdcall ThreadStarterFunc(void* aThread);

protected:
	SC_Semaphore mSemaphore;

	SC_ThreadHandle mThreadHandle;
	SC_ThreadId mThreadId;
	bool mIsRunning;

	static thread_local SC_Thread* gCurrentThread;
};
