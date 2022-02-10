
#include "SC_Thread.h"

#include <thread>

#if IS_WINDOWS_PLATFORM

static void SetThreadName(SC_ThreadHandle aThreadHandle, const wchar_t* aName)
{
	typedef HRESULT(WINAPI* SetThreadDescriptionType)(HANDLE aThreadHandle, PCWSTR ThreadDescription);
	static SetThreadDescriptionType SetThreadDescriptionFunc = (SetThreadDescriptionType)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "SetThreadDescription");
	if (SetThreadDescriptionFunc)
		SetThreadDescriptionFunc(aThreadHandle, aName);
}

#else
#error Platform not supported!
#endif


thread_local SC_Thread* SC_Thread::gCurrentThread = nullptr;

SC_Thread::SC_Thread()
	: mThreadHandle()
	, mThreadId(0)
	, mIsRunning(false)
{
#if IS_WINDOWS_PLATFORM
	mThreadHandle = (SC_ThreadHandle)_beginthreadex(nullptr, MB(1), &SC_Thread::ThreadStarterFunc, this, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	if (!mThreadHandle)
	{
		SC_ASSERT(false);
		return;
	}

	mThreadId = GetThreadId(mThreadHandle);
#else
#error Platform not supported!
#endif
}

SC_Thread::~SC_Thread()
{
	if (mIsRunning)
		Stop(true);

#if IS_WINDOWS_PLATFORM
	CloseHandle(mThreadHandle);
#else
#error Platform not supported!
#endif

}

void SC_Thread::Start()
{
	mIsRunning = true;

	if (!mThreadHandle)
		return;

#if IS_WINDOWS_PLATFORM
	::ResumeThread(mThreadHandle);
#else
#error Platform not supported!
#endif
}

void SC_Thread::Stop(bool aWaitForFinish /*= false*/)
{
	mIsRunning = false;
	if (aWaitForFinish)
		Wait();
}

void SC_Thread::Wait()
{
#if IS_WINDOWS_PLATFORM
	::WaitForSingleObjectEx(mThreadHandle, INFINITE, FALSE);
#else
#error Platform not supported!
#endif
}

bool SC_Thread::IsActive() const
{
	return mIsRunning;
}

SC_Semaphore& SC_Thread::GetSemaphore()
{
	static SC_Semaphore mainThreadSemaphore;

	return (GetMainThreadId() == GetCurrentThreadId()) ? mainThreadSemaphore : mSemaphore;
}

SC_ThreadHandle SC_Thread::GetHandle() const
{
	return mThreadHandle;
}

SC_ThreadId SC_Thread::GetId() const
{
	return mThreadId;
}

void SC_Thread::SetName(const char* aName)
{
	if (mThreadHandle)
	{
		std::string name(aName);
		std::wstring wname(name.begin(), name.end());
		SetThreadName(mThreadHandle, wname.c_str());
	}
}

void SC_Thread::Yield()
{
	std::this_thread::yield();
}

void SC_Thread::Sleep(uint32 aMilliseconds)
{
	if (aMilliseconds == 0)
		Yield();
	else
		std::this_thread::sleep_for(std::chrono::milliseconds(aMilliseconds));
}

SC_Thread* SC_Thread::GetCurrentThread()
{
	return gCurrentThread;
}

SC_ThreadId SC_Thread::GetCurrentThreadId()
{
#if IS_WINDOWS_PLATFORM
	return ::GetCurrentThreadId();
#else
#error "Unknown platform!"
#endif
}

struct MainThreadId
{
	MainThreadId()
	{
		gId = SC_Thread::GetCurrentThreadId();
	}
	static SC_ThreadId gId;
};
SC_ThreadId MainThreadId::gId = SC_UINT32_MAX;

SC_ThreadId SC_Thread::GetMainThreadId()
{
	static MainThreadId gMainThreadId;
	return MainThreadId::gId;
}

void SC_Thread::ThreadMain()
{
}

uint32 __stdcall SC_Thread::ThreadStarterFunc(void* aThread)
{
	SC_Thread* thread = reinterpret_cast<SC_Thread*>(aThread);
	SC_Thread::gCurrentThread = thread;

	{
		uint32 seed = uint32(uint64(time(0)) + uint64(thread));
		srand(seed);
		thread->ThreadMain();
		thread->mIsRunning = false;
	}

	SC_Thread::gCurrentThread = nullptr;

	return 0;
}
