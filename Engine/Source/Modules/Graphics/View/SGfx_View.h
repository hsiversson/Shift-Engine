#pragma once
#include "SGfx_ViewData.h"
#include "SGfx_Camera.h"

class SR_Texture;

class SGfx_ViewDataBufferer
{
public:
	SGfx_ViewDataBufferer(uint32 aNumBuffers = 2);
	~SGfx_ViewDataBufferer();

	uint32 GetNumBuffers() const;
	uint32 GetPrepareIndex() const;
	uint32 GetRenderIndex() const;

	void StartPrepare();
	void EndPrepare();
	void ResetPrepare();

	void StartRender();
	void EndRender();

	void WaitForPrepareTask(const SC_Future<bool>& aTaskEvent);

private:
	void FinishEndPrepare();
	void SyncWithRenderThread();

	void WaitForStartRenderEvent();
	void WaitForEndRenderEvent();

	const uint32 mNumBuffers;
	uint32 mPrepareIndex;
	uint32 mRenderIndex;

	uint32 mCurrentStartedPrepareFrame;
	uint32 mCurrentStartedRenderFrame;

	uint32 mPrepareIndexCounter;
	uint32 mFinishedPrepareIndexCounter;
	uint32 mRenderIndexCounter;
	uint32 mPendingRenderIndexCounter;

	uint32 mNumBuffersInFlight;

	SC_Ref<SC_Event> mStartRenderEvent;
	SC_Ref<SC_Event> mEndRenderEvent;
	SC_Mutex mRenderMutex;
	SC_ReadWriteMutex mWaitingForPrepareMutex;
	SC_Semaphore mFramesInFlightSemaphore;

	bool mIsPreparing;
	bool mIsRendering;
	bool mIsRenderingMainThread;
	bool mPendingEndPrepare;
	bool mIsReset;
	bool mStartedRenderingFrame;
};

class SGfx_View
{
public:
	SGfx_View();
	~SGfx_View();

	SGfx_ViewData& GetPrepareData();
	const SGfx_ViewData& GetRenderData(); // Should only be accessed from render tasks
	SGfx_ViewData& GetMutableRenderData(); // Should only be accessed from render tasks

	void SetCamera(const SGfx_Camera& aCamera);
	const SGfx_Camera& GetCamera() const;

	void SetOnlyDepth(bool aValue);
	bool IsOnlyDepth() const;
	void SetMainView(bool aValue);
	bool IsMainView() const;

	void SetOutputTexture(const SC_Ref<SR_Texture>& aTexture);
	SR_Texture* GetOutputTexture() const;

	void StartPrepare() { mViewDataBufferer.StartPrepare(); }
	void EndPrepare() { mViewDataBufferer.EndPrepare(); }
	void ResetPrepare() { mViewDataBufferer.ResetPrepare(); }

	void StartRender() { mViewDataBufferer.StartRender(); }
	void EndRender() { mViewDataBufferer.EndRender(); }
	void WaitForPrepareTask(const SC_Future<bool>& aTaskEvent) { mViewDataBufferer.WaitForPrepareTask(aTaskEvent); }

	void UpdateRenderSettings();
	SGfx_ViewRenderSettings& GetRenderSettings();
	const SGfx_ViewRenderSettings& GetRenderSettings() const;

private:
	SGfx_ViewDataBufferer mViewDataBufferer;
	SC_Array<SGfx_ViewData> mViewData;
	SGfx_ViewRenderSettings mRenderSettings;

	SGfx_Camera mCamera;
	SC_Ref<SR_Texture> mOutputTexture;
	bool mOnlyDepth;
	bool mIsMainView;
};

