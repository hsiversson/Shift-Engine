#include "SAF_Framework_Win64.h"

#if IS_WINDOWS_PLATFORM
#include "SAF_Window_Win64.h"
#include "Common/MessageQueue/SC_MessageQueue.h"
#include "RenderCore/Interface/SR_RenderDevice.h"
#include "InputOutput/File/SC_FileWatcher.h"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>

SAF_Framework_Win64::SAF_Framework_Win64()
	: mMessageQueue(SC_MessageQueue::Get())
{
	SC_FileWatcher::Create();
}

SAF_Framework_Win64::~SAF_Framework_Win64()
{
	SC_FileWatcher::Destroy();

	mMessageQueue = nullptr;
	SC_MessageQueue::Destroy();
}

void* SAF_Framework_Win64::GetNativeWindowHandle() const
{
	return (void*)SAF_WindowThread_Win64::Get()->GetMainWindowHandle();
}

float SAF_Framework_Win64::GetWindowDPI() const
{
	return SAF_WindowThread_Win64::Get()->GetMainWindow()->GetDPIScale();
}

bool SAF_Framework_Win64::Init()
{
	SC_CommandLine::Init(__argv, __argc);

	bool disableGraphics = /*!aCreateParams.mUseGraphics ||*/ SC_CommandLine::HasCommand("disablegraphics");
	bool showConsole = disableGraphics || SC_CommandLine::HasCommand("enableconsole");

	if (showConsole)
	{
		//if (aCreateParams.mAppType == SGFAppType::Server)
		//{
		//	SetConsoleTitle(aCreateParams.mApplicationName);
		//	SetConsoleCtrlHandler(ConsoleHandlerRoutine, true);
		//}
		//else
			CreateConsole(/*aCreateParams*/);
	}

	if (!SC_CommandLine::HasCommand("disabledpiawareness"))
	{
		if (HMODULE shcore = LoadLibrary(L"shcore.dll"))
		{
			typedef HRESULT(WINAPI* SetProcessDpiAwarenessFunc)(int);
			if (SetProcessDpiAwarenessFunc setProcessDpiAwarenessFunc = (SetProcessDpiAwarenessFunc)GetProcAddress(shcore, "SetProcessDpiAwareness"))
				setProcessDpiAwarenessFunc(2 /* Process_Per_Monitor_DPI_Aware */);

			FreeLibrary(shcore);
		}
	}

	// Create Console optionally
	// Init Render API
	// Create Window Thread
	// Create SwapChain
	// Set SwapChain

	SR_API renderApi = SR_API::D3D12;

#if SR_ENABLE_DX12
	if (SC_CommandLine::HasCommand("dx12"))
		renderApi = SR_API::D3D12;
#endif
#if SR_ENABLE_VULKAN
	if (SC_CommandLine::HasCommand("vulkan"))
		renderApi = SR_API::Vulkan;
#endif

	if (!SR_RenderDevice::Create(renderApi))
	{
		assert(false && "Failed to init RenderDevice.");
		return false;
	}

	SAF_WindowCreateParams windowInitParams;
	windowInitParams.mName = "Shift Engine";

	SAF_Window_Win64::State windowState;
	windowState.mFullscreen = false;
	windowState.mExclusiveMode = false;
	windowState.mAllowBorders = true;
	windowState.mAllowResize = true;
	windowState.mPosition = { 50, 50 };
	windowState.mClientAreaSize = { 2560, 1440 };
	windowState.mStatus = SAF_Window_Win64::State::Status_Normal;
	windowState.mChangeFlags = 0;
	if (!SAF_WindowThread_Win64::Init(windowInitParams, windowState))
	{
		assert(false && "Failed to init window thread.");
		return false;
	}

	SAF_Window_Win64* window = SAF_WindowThread_Win64::Get()->GetMainWindow();
	// get window state*
	windowState = window->GetWindowState_MainThread();

	SR_SwapChainProperties swapChainProperties;
	swapChainProperties.mFormat = SR_Format::RGBA8_UNORM;
	swapChainProperties.mFullscreen = false;
	swapChainProperties.mTripleBuffer = false;
	swapChainProperties.mSize = SC_Max(windowState.mClientAreaSize, SC_IntVector2(1));

	window->mSwapChain = SR_RenderDevice::gInstance->CreateSwapChain(swapChainProperties, window->GetHWND());
	if (!window->mSwapChain)
	{
		return false;
	}
	SR_RenderDevice::gInstance->SetSwapChain(window->mSwapChain);

	return SAF_Framework::Init();
}

bool SAF_Framework_Win64::Update()
{
	SC_PROFILER_FUNCTION();
	{
		MSG msg;
		msg.message = 0;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&msg);

			switch (msg.message)
			{
			case SAF_WindowThreadMsg_Quit:
				RequestExit();
				break;

			case SAF_WindowThreadMsg_Alt_Enter:
				break;
			}
		}

		mMessageQueue->Process();
	}

	// Sync Window State
	SyncWindowState();
	ApplyWindowStateChanges();

	bool result = SAF_Framework::Update();
	if (result)
	{

	}

	return result;
}

void SAF_Framework_Win64::Exit()
{

}

bool SAF_Framework_Win64::CreateConsole()
{
	if (AttachConsole((DWORD)-1) == 0)
	{
		if (AllocConsole() == 0)
			return false;
	}

	//SetConsoleTitle(aCreateParams.mApplicationName);

	bool redirectStdStreams = !SC_CommandLine::HasCommand("disablestdstreamredirect");
	if (redirectStdStreams)
	{
		HANDLE stdHandle;
		int32 hConsole;
		FILE* fp;

		// Unbuffered STDOUT to the console
		stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		fp = _fdopen(hConsole, "w");
		freopen_s(&fp, "CONOUT$", "w", stdout);

		// Unbuffered STDIN to the console
		stdHandle = GetStdHandle(STD_INPUT_HANDLE);
		hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		fp = _fdopen(hConsole, "w");
		freopen_s(&fp, "CONIN$", "w", stdin);

		// Unbuffered STDERR to the console
		stdHandle = GetStdHandle(STD_ERROR_HANDLE);
		hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		fp = _fdopen(hConsole, "w");
		freopen_s(&fp, "CONOUT$", "w", stderr);
	}

	//SetConsoleCtrlHandler(ConsoleHandlerRoutine, true);
	return true;
}

void SAF_Framework_Win64::SyncWindowState()
{
	if (SAF_Window_Win64* window = SAF_WindowThread_Win64::Get()->GetMainWindow())
	{
		SAF_Window_Win64::State windowState = window->GetWindowState_MainThread();
		bool isFullscreenExclusive = (windowState.mFullscreen && windowState.mExclusiveMode);
		bool shouldRecreateSwapChain = false;
		bool needSwapChainUpdate = window->mUpdateSwapChain;

		if ((windowState.mChangeFlags != 0) || needSwapChainUpdate)
		{
			SC_Ref<SR_SwapChain>& swapChain = window->mSwapChain;
			if (swapChain)
			{
				if (windowState.mChangeFlags & SAF_Window_Win64::State::ChangeFlag_BeginMove)
					window->mIsCurrentlyMoving = true;

				if (windowState.mChangeFlags & SAF_Window_Win64::State::ChangeFlag_EndMove)
					window->mIsCurrentlyMoving = false;

				if (windowState.mChangeFlags & SAF_Window_Win64::State::ChangeFlag_Mode)
					shouldRecreateSwapChain = true;

				if (windowState.mChangeFlags & SAF_Window_Win64::State::ChangeFlag_Size)
				{
					if (windowState.mClientAreaSize != swapChain->GetProperties().mSize)
					{
						needSwapChainUpdate = true;
						//shouldRecreateSwapChain |= isFullscreenExclusive;
					}
				}

				if (window->mIsCurrentlyMoving && (needSwapChainUpdate /*|| needSwapChainUpdateHDR*/) && !shouldRecreateSwapChain)
				{
					needSwapChainUpdate = false;
					//needSwapChainUpdateHDR = false;
					window->mUpdateSwapChain = true;
				}


				if (shouldRecreateSwapChain)
				{
					SR_SwapChainProperties props = swapChain->GetProperties();

					if (!isFullscreenExclusive)
					{
						//props.mRefreshRate = SVector2(0, 1);
						//if (windowState.mChangeFlags & SAF_Window_Win64::State::ChangeFlag_ExclusiveFullscreen)
						//	swapChain->Recreate(props);

						//if (!windowState.mFullscreen)
						//{
						//	windowState.mPosition = windowState.myNormalPosition;
						//	windowState.mClientAreaSize = windowState.myNormalSize;
						//}
						//SWindowThreadWin64::GetInstance()->SetWindowStateBlocking(window, &windowState);
						//window->mNeedAdditionalSwapChainUpdate = true;
					}
					else
					{
						//props.mSize = windowState.myFullscreenSize;
						//props.myRefreshRate = windowState.myFullscreenHz;

						//if (!swapChain->Recreate(props) || !swapChain->IsFullscreen())
						//{
						//	windowState.mChangeFlags |= SAF_Window_Win64::State::ChangeFlag_Settings;
						//	windowState.mExclusiveMode = false;
						//	windowState.mFullscreen = false;
						//	//SWindowThreadWin64::GetInstance()->SetWindowStateBlocking(window, &windowState);
						//}
						//
						//window->mNeedAdditionalSwapChainUpdate = true;
					}
				}
				else if (needSwapChainUpdate /*|| updateSwapChainHdr*/)
				{
					SR_SwapChainProperties props = swapChain->GetProperties();
					props.mSize = windowState.mClientAreaSize;
					swapChain->Update(props);
				}
			}
		}
	}
}

void SAF_Framework_Win64::ApplyWindowStateChanges()
{
	SAF_WindowThread_Win64* windowThread = SAF_WindowThread_Win64::Get();
	if (SAF_Window_Win64* window = windowThread->GetMainWindow())
	{
		SAF_Window_Win64::State state = window->GetWindowState_MainThread();

		if (state.mFullscreen != mIsFullscreen)
		{
			state.mFullscreen = mIsFullscreen;
			windowThread->SetWindowState(window, state);
		}
	}
}

#endif