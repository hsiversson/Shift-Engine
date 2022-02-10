#pragma once
#include "Platform/Async/SC_Thread.h"

#if IS_WINDOWS_PLATFORM
class SR_SwapChain;

struct SAF_WindowCreateParams
{
	std::string mName;
	SC_IntVector2 mResolution;
};

class SAF_Window_Win64
{
	friend class SAF_WindowThread_Win64;
public:
	struct State
	{
		State() { SC_ZeroMemory(this, sizeof(State)); }

		enum ChangeFlag
		{
			ChangeFlag_Position = 0x1,
			ChangeFlag_Size = 0x2,
			ChangeFlag_Paint = 0x4,
			ChangeFlag_Status = 0x8,
			ChangeFlag_BeginMove = 0x10,
			ChangeFlag_EndMove = 0x20,
			ChangeFlag_WaitForPresent = 0x40,
			ChangeFlag_ExclusiveFullscreen = 0x80,
			ChangeFlag_Settings = 0x100,
			ChangeFlag_Monitor = 0x200,
			ChangeFlag_Mode = 0x400,
		};
	
		enum Status
		{
			Status_Hidden,
			Status_Minimized,
			Status_NotInFocus,
			Status_Normal,
		};
	
		SC_IntVector2 mClientAreaSize;
		SC_IntVector2 mPosition;
	
		Status mStatus;
	
		uint32 mChangeFlags;
	
		bool mFullscreen;
		bool mAllowBorders;
		bool mAllowResize;
		bool mExclusiveMode;
	};

public:
	SAF_Window_Win64();
	SAF_Window_Win64(const char* aName, const State& aState, void* aParentWindowHandle = nullptr);
	~SAF_Window_Win64();

	void ApplyMode();

	HWND GetHWND() const;
	const State& GetWindowState_MainThread();

	float GetDPIScale() const;

	SC_Ref<SR_SwapChain> mSwapChain;
	bool mIsCurrentlyMoving;
	bool mUpdateSwapChain;
private:
	void HandlePositionChanged();

	void SyncState(uint32 aChangeFlags = 0);

	void SetStatus(const State::Status& aStatus);

	void SetDPIScale(float aScale);
private:
	SC_Mutex mWindowStatesMutex;
	State mCurrentWindowState;
	State mSharedWindowState;
	State mMainThreadWindowState;
	State mLatestSyncedWindowState;
	HWND mWindowHandle;

	float mDPIScale;
};

//////////////////////////////////////////////////////
// Window Thread

enum SAF_WindowThreadMsg
{
	SAF_WindowThreadMsg_Quit = (WM_APP + 1),
	SAF_WindowThreadMsg_Alt_Enter,

	SAF_WindowThreadMsg_ShowCursor,
	SAF_WindowThreadMsg_SetCursor,
	SAF_WindowThreadMsg_SetMode,
	SAF_WindowThreadMsg_SetState,
	SAF_WindowThreadMsg_RestoreDisplayMode,
	SAF_WindowThreadMsg_CreateWindow,
	SAF_WindowThreadMsg_DestroyWindow,
	SAF_WindowThreadMsg_EnableWindow,
	SAF_WindowThreadMsg_SetCapture,
	SAF_WindowThreadMsg_ReleaseCapture,

	SAF_WindowThreadMsg_WaitForMessages,
};

class SAF_WindowThread_Win64 : public SC_Thread
{
public:
	static SAF_WindowThread_Win64* Get() { return gInstance; }
	static bool Init(SAF_WindowCreateParams& aInitParams, const SAF_Window_Win64::State& aWindowState);
	static void Destroy();

	void Stop(bool aWaitForFinish = false) override;

	SAF_Window_Win64* GetMainWindow() const { return mMainWindow; }
	HWND GetMainWindowHandle() const { return mMainWindow->GetHWND(); }
	const wchar_t* GetWndClassName() const;

protected:
	SAF_WindowThread_Win64();
	~SAF_WindowThread_Win64();

	void ThreadMain() override;

private:
	bool CreateMainWindow();

	void SendMessageToWindowThread(uint32 aMsg, WPARAM aWPARAM = 0, LPARAM aLPARAM = 0, HWND aHwnd = 0);
	void PostMessageToWindowThread(uint32 aMsg, WPARAM aWPARAM = 0, LPARAM aLPARAM = 0, HWND aHwnd = 0);
	void PostMessageToMainThread(uint32 aMsg, WPARAM aWPARAM = 0, LPARAM aLPARAM = 0);
	static LRESULT CALLBACK	WndProc(HWND aHwnd, UINT aMsg, WPARAM aWPARAM, LPARAM aLPARAM);
	static SAF_WindowThread_Win64* gInstance;

private:
	SAF_WindowCreateParams mWindowCreateParams;
	SAF_Window_Win64::State mTargetWindowState;

	SC_Event mInitEvent;
	HANDLE mWakeFromSleepEvent;
	HCURSOR mCursor;

	std::wstring mWndClassName;
	SAF_Window_Win64* mMainWindow;
};
#endif