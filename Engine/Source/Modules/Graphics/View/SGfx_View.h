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

	void StartRender();
	void EndRender();

private:
	const uint32 mNumBuffers;
	uint32 mPrepareIndex;
	uint32 mRenderIndex;

	uint32 mCurrentStartedPrepareFrame;
	uint32 mCurrentStartedRenderFrame;

	uint32 mPrepareIndexCounter;
	uint32 mRenderIndexCounter;
	uint32 mPendingRenderIndexCounter;

	uint32 mNumBuffersInFlight;

	SC_Event mStartRenderEvent;
	SC_Event mEndRenderEvent;
	SC_Mutex mRenderMutex;

	bool mIsPreparing;
	bool mIsRendering;
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

	void StartRender() { mViewDataBufferer.StartRender(); }
	void EndRender() { mViewDataBufferer.EndRender(); }

private:
	SGfx_ViewDataBufferer mViewDataBufferer;
	SC_Array<SGfx_ViewData> mViewData;
	SGfx_Camera mCamera;
	SC_Ref<SR_Texture> mOutputTexture;
	bool mOnlyDepth;
	bool mIsMainView;
};

