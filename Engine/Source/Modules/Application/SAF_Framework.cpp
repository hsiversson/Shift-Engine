#include "SAF_Framework.h"
#include "Platform/Time/SC_Time.h"
#include "RenderCore/Interface/SR_RenderDevice.h"
#include "RenderCore/Interface/SR_SwapChain.h"

#if IS_WINDOWS_PLATFORM
#include "Application/Win64/SAF_Framework_Win64.h"
#endif

SAF_Framework* SAF_Framework::gInstance = nullptr;

SAF_Framework::SAF_Framework()
	: mCallbacks(nullptr)
	, mExitRequested(false)
{

}

SAF_Framework::~SAF_Framework()
{

}

int SAF_Framework::AppMain()
{
	// Register MainThread
	SC_Thread::GetMainThreadId();
#if ENABLE_LOGGING
	SC_Logger::Create();
#endif
	SC_ThreadPool::Create();
#if ENABLE_CONSOLE
	SC_Console::Create();
#endif

	if (!Init())
		return -1;

	MainLoop();
	Exit();


#if ENABLE_CONSOLE
	SC_Console::Destroy();
#endif
	SC_ThreadPool::Destroy();
#if ENABLE_LOGGING
	SC_Logger::Destroy();
#endif

	return 0;
}

void* SAF_Framework::GetNativeWindowHandle() const
{
	return nullptr;
}

float SAF_Framework::GetWindowDPI() const
{
	return 1.0f;
}

SAF_Framework* SAF_Framework::Create(const SAF_FrameworkCreateParams& aCreateParams)
{
#if IS_WINDOWS_PLATFORM
	gInstance = new SAF_Framework_Win64();
#else
#error Platform not supported yet!
#endif

	gInstance->mCallbacks = aCreateParams.mCallbacks;

	return gInstance;
}

SAF_Framework* SAF_Framework::Get()
{
	return gInstance;
}

void SAF_Framework::Destroy()
{
	delete gInstance;
	gInstance = nullptr;
}

bool SAF_Framework::Init()
{
	if (mCallbacks)
	{
		if (!mCallbacks->Init())
			return false;
	}

	return true;
}

bool SAF_Framework::Update()
{
	// BeginFrame Swapchain
	// BeginFrame Input
	// BeginFrame ThreadPool

	SC_Time::Get()->Update();

	if (mCallbacks)
		mCallbacks->Update();

	// Update modules

	if (SR_RenderDevice::gInstance)
	{
		if (mCallbacks)
			mCallbacks->Render();

		// Render Modules

		SR_RenderDevice::gInstance->Present();
		SR_RenderDevice::gInstance->mLatestFinishedFrame = SC_Time::gFrameCounter;
	}

	// EndFrame ThreadPool
	// EndFrame Input
	// EndFrame Swapchain

	return !mExitRequested;
}

void SAF_Framework::Exit()
{
	if (mCallbacks)
		mCallbacks->Exit();
}

void SAF_Framework::RequestExit()
{
	mExitRequested = true;
}

void SAF_Framework::MainLoop()
{
	SC_Time timer;
	while (Update());
}
