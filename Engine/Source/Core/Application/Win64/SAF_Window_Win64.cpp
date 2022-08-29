#include "SAF_Window_Win64.h"

#if IS_WINDOWS_PLATFORM
#include "RenderCore/ImGui/SR_ImGui.h"
#include "Common/MessageQueue/SC_MessageQueue.h"

static DWORD GetWindowStyle(const SAF_Window_Win64::State& aState)
{
	DWORD windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	if (!aState.mFullscreen) // windowed
		windowStyle |= aState.mAllowBorders ? ((aState.mAllowResize ? WS_OVERLAPPEDWINDOW : (WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX))) : 0;
	else if (!aState.mExclusiveMode) // borderless fullscreen
		windowStyle |= WS_POPUP;
	else // exclusive fullscreen
		windowStyle |= WS_POPUP;

	return windowStyle;
}

SAF_Window_Win64::SAF_Window_Win64()
	: mIsCurrentlyMoving(false)
	, mUpdateSwapChain(false)
	, mWindowHandle(nullptr)
{

}

SAF_Window_Win64::SAF_Window_Win64(const char* aName, const State& aState, void* aParentWindowHandle /*= nullptr*/)
	: mIsCurrentlyMoving(false)
	, mUpdateSwapChain(false)
	, mWindowHandle(nullptr)
	, mDPIScale(1.0f)
{
	const SC_IntVector2& position = aState.mPosition;
	RECT windowRect = { (LONG)position.x, (LONG)position.y, LONG(position.x + aState.mClientAreaSize.x), LONG(position.y + aState.mClientAreaSize.y) };
	DWORD windowStyle = GetWindowStyle(aState);
	if (aState.mStatus != State::Status_Hidden)
		windowStyle |= WS_VISIBLE;

	::AdjustWindowRect(&windowRect, windowStyle, FALSE);

	std::wstring name(SC_UTF8ToUTF16(aName));

	mWindowHandle = ::CreateWindow(
		SAF_WindowThread_Win64::Get()->GetWndClassName(),
		name.c_str(),
		windowStyle,
		windowRect.left,
		windowRect.top,
		(windowRect.right - windowRect.left),
		(windowRect.bottom - windowRect.top),
		(HWND)aParentWindowHandle,
		nullptr,
		::GetModuleHandle(NULL),
		nullptr
	);

	if (!mWindowHandle)
		return;

	SetWindowLongPtr(mWindowHandle, GWLP_USERDATA, LONG_PTR(this));
	mCurrentWindowState = aState;
	ApplyMode();
	::UpdateWindow(mWindowHandle);
	mMainThreadWindowState = mSharedWindowState;

	if (HMODULE shcore = LoadLibrary(L"shcore.dll"))
	{
		typedef HRESULT(WINAPI* GetDpiForMonitorFn)(HMONITOR aHMonitor, int aDPIType, UINT* aDPIx, UINT* aDPIy);
		GetDpiForMonitorFn getDpiForMonitor = reinterpret_cast<GetDpiForMonitorFn>(GetProcAddress(shcore, "GetDpiForMonitor"));
		if (getDpiForMonitor)
		{
			HMONITOR hMonitor = MonitorFromWindow(mWindowHandle, MONITOR_DEFAULTTOPRIMARY);
			UINT dpi[2] = { 1, 1 };
			HRESULT hr = getDpiForMonitor(hMonitor, 0, &dpi[0], &dpi[1]);
			if (SUCCEEDED(hr))
				mDPIScale = (dpi[0] / (float)USER_DEFAULT_SCREEN_DPI);
		}

		FreeLibrary(shcore);
	}
}

SAF_Window_Win64::~SAF_Window_Win64()
{

}

void SAF_Window_Win64::ApplyMode()
{
	HMONITOR monitor = ::MonitorFromWindow(mWindowHandle, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monitorInfo = {};
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	::GetMonitorInfo(monitor, &monitorInfo);

	DEVMODE devMode;
	ZeroMemory(&devMode, sizeof(devMode));
	devMode.dmSize = sizeof(devMode);
	::EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

	if ((mCurrentWindowState.mStatus == State::Status_Minimized) && ::IsIconic(mWindowHandle))
		ShowWindow(mWindowHandle, SW_MINIMIZE);

	//if (!mCurrentWindowState.mAllowFullscreen)
	//	mCurrentWindowState.mFullscreen = false;
	//
	//if (!mCurrentWindowState.mAllowExclusiveMode)
	//	mCurrentWindowState.mExclusiveMode = false;

	DWORD style = WS_VISIBLE | GetWindowStyle(mCurrentWindowState);

	if (mCurrentWindowState.mFullscreen)
	{
		// fullscreen - resize our borderless window to fill closest monitor
		SC_IntVector2 pos(
			monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.top);

		SC_IntVector2 size(
			devMode.dmPelsWidth ? devMode.dmPelsWidth : (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left),
			devMode.dmPelsHeight ? devMode.dmPelsHeight : (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top));

		//SC_LOG("ApplyMode: Borderless, pos:%i,%i size:%i,%i style:%08x", pos.x, pos.y, size.x, size.y);

		// Hack to fix taskbar covering our window in fake fullscreen mode
		// Remove the WS_VISIBLE bit, then we set the window visible again (below with SWP_SHOWWINDOW).
		// Apparently this causes Windows to re-evaluate whether or not the window is "fullscreen",
		// and specifically, whether the taskbar should be behind our window or not.
		::SetWindowLongPtrW(mWindowHandle, GWL_STYLE, style & ~WS_VISIBLE);
		::SetWindowPos(mWindowHandle, 0, pos.x, pos.y, size.x, size.y, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}


	if (::IsIconic(mWindowHandle))
		ShowWindow(mWindowHandle, SW_RESTORE);

	if (mCurrentWindowState.mStatus == State::Status_Normal)
		::SetForegroundWindow(mWindowHandle);

	SyncState();
}

HWND SAF_Window_Win64::GetHWND() const
{
	return mWindowHandle;
}

const SAF_Window_Win64::State& SAF_Window_Win64::GetWindowState_MainThread()
{
	SC_MutexLock lock(mWindowStatesMutex);
	mMainThreadWindowState = mSharedWindowState;
	mSharedWindowState.mChangeFlags = 0;

	return mMainThreadWindowState;
}

float SAF_Window_Win64::GetDPIScale() const
{
	return mDPIScale;
}

void SAF_Window_Win64::HandlePositionChanged()
{
	HWND hWnd = (HWND)mWindowHandle;
	if (::IsIconic(hWnd) && (mCurrentWindowState.mStatus != State::Status_Minimized))
		SetStatus(State::Status_Minimized);

	WINDOWPLACEMENT placement;
	placement.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(hWnd, &placement);
	if ((placement.showCmd == SW_SHOWNORMAL) || (placement.showCmd == SW_SHOWMAXIMIZED))
	{
		RECT borders = { 0, 0, 0, 0 };
		DWORD windowStyle = static_cast<DWORD>(GetWindowLong(hWnd, GWL_STYLE));
		::AdjustWindowRect(&borders, windowStyle, FALSE);

		RECT rect;
		::GetWindowRect(hWnd, &rect);
		rect.left -= borders.left;
		rect.right -= borders.right;
		rect.top -= borders.top;
		rect.bottom -= borders.bottom;
		mCurrentWindowState.mPosition = SC_IntVector2(rect.left, rect.top);
		mCurrentWindowState.mClientAreaSize = SC_IntVector2(rect.right - rect.left, rect.bottom - rect.top);
	}

	SyncState();
}

void SAF_Window_Win64::SyncState(uint32 aChangeFlags)
{
	if ((aChangeFlags == 0) && memcmp(&mCurrentWindowState, &mLatestSyncedWindowState, sizeof(State)) == 0)
		return;

	uint32 changes = aChangeFlags;

	if (mCurrentWindowState.mPosition != mSharedWindowState.mPosition)
		changes |= (State::ChangeFlag_Position | State::ChangeFlag_Settings);

	if (mCurrentWindowState.mClientAreaSize != mSharedWindowState.mClientAreaSize)
		changes |= (State::ChangeFlag_Size | State::ChangeFlag_Settings);

	if (changes)
	{
		SC_MutexLock lock(mWindowStatesMutex);
		uint32 prevChanges = mSharedWindowState.mChangeFlags;
		mSharedWindowState = mCurrentWindowState;
		mSharedWindowState.mChangeFlags = changes | prevChanges;
	}
	mLatestSyncedWindowState = mCurrentWindowState;
}

void SAF_Window_Win64::SetStatus(const State::Status& aStatus)
{
	bool wasVisible = mCurrentWindowState.mStatus >= State::Status_NotInFocus;
	bool isVisible = aStatus >= State::Status_NotInFocus;
	mCurrentWindowState.mStatus = aStatus;

	if (wasVisible != isVisible)
		ApplyMode();

	SyncState();
}

void SAF_Window_Win64::SetDPIScale(float aScale)
{
	mDPIScale = aScale;
}

SAF_WindowThread_Win64* SAF_WindowThread_Win64::gInstance = nullptr;

bool SAF_WindowThread_Win64::Init(SAF_WindowCreateParams& aInitParams, const SAF_Window_Win64::State& aWindowState)
{
	assert(!gInstance);
	gInstance = new SAF_WindowThread_Win64;
	gInstance->SetName("Window Thread");
	gInstance->mTargetWindowState = aWindowState;
	gInstance->mWindowCreateParams = aInitParams;
	gInstance->Start();

	gInstance->mInitEvent.Wait();

	return true;
}

void SAF_WindowThread_Win64::Destroy()
{

}

void SAF_WindowThread_Win64::Stop(bool /*aWaitForFinish*/)
{
	SC_Thread::Stop();
	SetEvent(mWakeFromSleepEvent);
}

bool SAF_WindowThread_Win64::SetWindowState(SAF_Window_Win64* aWindow, const SAF_Window_Win64::State& aState)
{
	SendMessageToWindowThread(SAF_WindowThreadMsg_SetState, (WPARAM)&aState, 0, aWindow ? aWindow->GetHWND() : nullptr);
	return true;
}

const wchar_t* SAF_WindowThread_Win64::GetWndClassName() const
{
	return mWndClassName.c_str();
}

SAF_WindowThread_Win64::SAF_WindowThread_Win64()
	: mMainWindow(nullptr)
{
	mWakeFromSleepEvent = CreateEvent(nullptr, false, false, L"SAF_WindowThread_Win64::WakeFromSleepEvent");

	mCursor = LoadCursor(0, IDC_ARROW);
}

SAF_WindowThread_Win64::~SAF_WindowThread_Win64()
{
	CloseHandle(mWakeFromSleepEvent);
}

void SAF_WindowThread_Win64::ThreadMain()
{
	bool initResult = CreateMainWindow();
	mInitEvent.Signal();
	if (!initResult)
	{
		Stop();
		return;
	}

	while (mIsRunning)
	{
		MSG msg;
		msg.message = 0;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		MsgWaitForMultipleObjects(1, &mWakeFromSleepEvent, false, 1000, QS_ALLINPUT);
	}
}

bool SAF_WindowThread_Win64::CreateMainWindow()
{
	mWndClassName = L"ShiftEngineWindowClass";

	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = nullptr;//reinterpret_cast<HICON>(aParams.myNativeAppIcon);
	wcex.hCursor = mCursor;
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = mWndClassName.c_str();
	wcex.hIconSm = 0;	//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	if (!RegisterClassEx(&wcex))
	{
		SC_ERROR("Register Window Class Failed with code: %d", GetLastError());
		return false;
	}

	mMainWindow = new SAF_Window_Win64(mWindowCreateParams.mName.c_str(), mTargetWindowState);
	return (mMainWindow != nullptr);
}

void SAF_WindowThread_Win64::SendMessageToWindowThread(uint32 aMsg, WPARAM aWPARAM, LPARAM aLPARAM, HWND aHwnd)
{
	::SendMessage(aHwnd ? aHwnd : GetMainWindowHandle(), aMsg, aWPARAM, aLPARAM);
}

void SAF_WindowThread_Win64::PostMessageToWindowThread(uint32 aMsg, WPARAM aWPARAM, LPARAM aLPARAM, HWND aHwnd)
{
	::PostMessage(aHwnd ? aHwnd : GetMainWindowHandle(), aMsg, aWPARAM, aLPARAM);
}

void SAF_WindowThread_Win64::PostMessageToMainThread(uint32 aMsg, WPARAM aWPARAM, LPARAM aLPARAM)
{
	::PostThreadMessage(SC_Thread::GetMainThreadId(), aMsg, aWPARAM, aLPARAM);
}

LRESULT CALLBACK SAF_WindowThread_Win64::WndProc(HWND aHwnd, UINT aMsg, WPARAM aWPARAM, LPARAM aLPARAM)
{
	if (!gInstance)
		return DefWindowProcW(aHwnd, aMsg, aWPARAM, aLPARAM);

	SAF_Window_Win64* window = reinterpret_cast<SAF_Window_Win64*>(GetWindowLongPtr(aHwnd, GWLP_USERDATA));
	if (!window)
		return DefWindowProcW(aHwnd, aMsg, aWPARAM, aLPARAM);

	switch (aMsg)
	{
		// Custom Messgaes
	case SAF_WindowThreadMsg_ShowCursor:
	{
		if (aWPARAM != 0)
			while (::ShowCursor(true) < 0);
		else
			while (::ShowCursor(false) >= 0);

		return 0;
	}

	//case SAF_WindowThreadMsg_SetCursor:
	//{
	//	HCURSOR cursor = (HCURSOR)aWPARAM;
	//	gInstance->mCursor = cursor;
	//	::SetCursor(cursor);
	//
	//	return 0;
	//}

	case SAF_WindowThreadMsg_RestoreDisplayMode:
	{
		::SetCursor(LoadCursor(0, IDC_ARROW));
		::ShowCursor(true);
		::ClipCursor(nullptr);

		return 0;
	}

	// Windows Messages

	case WM_ENTERSIZEMOVE:
	{
		window->SyncState(SAF_Window_Win64::State::ChangeFlag_BeginMove);
		return 0;
	}

	case WM_EXITSIZEMOVE:
	{
		window->SyncState(SAF_Window_Win64::State::ChangeFlag_EndMove);
		return 0;
	}

	case WM_ACTIVATE:
	{
		SAF_Window_Win64::State::Status windowStatus = SAF_Window_Win64::State::Status_NotInFocus;

		if (HIWORD(aWPARAM) != 0) // minimized
			windowStatus = SAF_Window_Win64::State::Status_Minimized;
		else if (LOWORD(aWPARAM) != 0) // activated
			windowStatus = SAF_Window_Win64::State::Status_Normal;

		window->SetStatus(windowStatus);

		break;
	}

	case WM_WINDOWPOSCHANGING:
	{
		//WINDOWPOS* windowPos = (WINDOWPOS*)aLPARAM;
		//if (window->HandlePositionChanging(windowPos))
		//	return 0;
		break;
	}

	case WM_WINDOWPOSCHANGED:
	{
		window->HandlePositionChanged();
		return 0;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		if (BeginPaint(aHwnd, &ps))
			EndPaint(aHwnd, &ps);
		window->SyncState(SAF_Window_Win64::State::ChangeFlag_Paint);
		return 0;
	}

	case WM_SYSCOMMAND:
	{
		int32 cmd = GET_SC_WPARAM(aWPARAM);

		if ((cmd == SC_CLOSE) && (GetKeyState(VK_CONTROL) < 0) && (GetKeyState(VK_SHIFT) < 0))
		{
			//MC_Misc::Exit(0);
		}

		if (cmd == SC_KEYMENU)
			return 0;

		break;
	}

	//case WM_SYSKEYDOWN:
	//{
	//	if (aWPARAM == VK_RETURN && (HIWORD(aLPARAM) & KF_ALTDOWN))
	//	{
	//		// ALT + ENTER
	//	}
	//	break;
	//}

	case WM_SETCURSOR:
	{
		if (LOWORD(aLPARAM) == HTCLIENT)
		{
			::SetCursor(gInstance->mCursor);
			return 1;
		}
		break;
	}

	case WM_CLOSE:
	{
		if (window == gInstance->mMainWindow)
		{
			gInstance->PostMessageToMainThread(SAF_WindowThreadMsg_Quit);
		}
		return 0;
	}

	case WM_DPICHANGED:
	{
		uint32 dpi = (uint32)LOWORD(aWPARAM);
		float dpiScale = dpi / (float)USER_DEFAULT_SCREEN_DPI;

		const RECT* rect = (RECT*)aLPARAM;
		::SetWindowPos(window->GetHWND(), nullptr, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, 0);

		window->SetDPIScale(dpiScale);
		SR_ImGui::Get()->SetDPIScale(dpiScale);
		window->HandlePositionChanged();
		return 0;
	}

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{
		SC_MouseKeyMessageData data = {};
		if (aMsg == WM_LBUTTONDOWN || aMsg == WM_RBUTTONDOWN || aMsg == WM_MBUTTONDOWN)
			data.mState = SC_InputKeyStateMessage::Pressed;
		else
			data.mState = SC_InputKeyStateMessage::Released;

		if (aMsg == WM_LBUTTONDOWN || aMsg == WM_LBUTTONUP) data.mKey = SC_MouseKeyCode::Left;
		else if (aMsg == WM_RBUTTONDOWN || aMsg == WM_RBUTTONUP) data.mKey = SC_MouseKeyCode::Right;
		else if (aMsg == WM_MBUTTONDOWN || aMsg == WM_MBUTTONUP) data.mKey = SC_MouseKeyCode::Middle;

		SC_Message msg;
		msg.mType = SC_MessageType::Mouse;
		msg.Store(data);
		SC_MessageQueue::QueueMessage(msg);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		SC_Message msg;
		msg.mType = SC_MessageType::Scroll;
		float scrollDelta = (float)(GET_WHEEL_DELTA_WPARAM(aWPARAM) / WHEEL_DELTA);
		msg.Store(scrollDelta);
		SC_MessageQueue::QueueMessage(msg);
		break;
	}
	case WM_MOUSEMOVE:
	{
		SC_Message msg;
		msg.mType = SC_MessageType::MousePos;
		msg.Store((uint32)aLPARAM);
		SC_MessageQueue::QueueMessage(msg);
		break;
	}
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (aWPARAM < 256)
		{
			SC_KeyMessageData outData;
			outData.mState = SC_InputKeyStateMessage::Pressed;
			outData.mKey = SC_ConvertToKeyCode((uint32)aWPARAM);

			SC_Message msg;
			msg.mType = SC_MessageType::Key;
			msg.Store(outData);
			SC_MessageQueue::QueueMessage(msg);
		}
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		if (aWPARAM < 256)
		{
			SC_KeyMessageData outData;
			outData.mState = SC_InputKeyStateMessage::Released;
			outData.mKey = SC_ConvertToKeyCode((uint32)aWPARAM);

			SC_Message msg;
			msg.mType = SC_MessageType::Key;
			msg.Store(outData);
			SC_MessageQueue::QueueMessage(msg);
		}
		break;
	}
	case WM_CHAR:
	{
		if (aWPARAM > 0 && aWPARAM < 0x10000)
		{
			SC_Message msg;
			msg.mType = SC_MessageType::InputChar;
			msg.Store((uint16)aWPARAM);
			SC_MessageQueue::QueueMessage(msg);
		}
		break;
	}

	case SAF_WindowThreadMsg_SetState:
	{
		SAF_Window_Win64::State* state = (SAF_Window_Win64::State*)aWPARAM;

		if (window->mCurrentWindowState.mStatus != state->mStatus)
			window->SetStatus((SAF_Window_Win64::State::Status)state->mStatus);

		window->mCurrentWindowState = *state;
		window->ApplyMode();
	}
	return 0;

	default:
		//SInput::WndProc(aHwnd, aMsg, aWPARAM, aLPARAM);
		break;
	}

	// Run Editor WndProc
	// Run Input WndProc

	return DefWindowProcW(aHwnd, aMsg, aWPARAM, aLPARAM);
}


#endif