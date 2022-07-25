#pragma once
#include "SC_Semaphore.h"
#include <string>

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
	std::string GetName() const;

	static void Yield();
	static void Sleep(uint32 aMilliseconds);

	static void RegisterMainThread();
	static SC_ThreadId GetMainThreadId();
	static SC_Thread* GetCurrentThread();
	static SC_ThreadId GetCurrentThreadId();
	static std::string GetCurrentThreadName();

	static thread_local bool gIsMainThread;
	static thread_local bool gIsTaskThread;
	static thread_local bool gIsRenderThread;

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

