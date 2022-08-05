#include "SGfx_View.h"
#include "Graphics/PostEffects/SGfx_PostEffects.h"

SGfx_ViewDataBufferer::SGfx_ViewDataBufferer(uint32 aNumBuffers)
	: mNumBuffers(aNumBuffers)
	, mPrepareIndex(0)
	, mRenderIndex(0)
	, mCurrentStartedPrepareFrame(0)
	, mCurrentStartedRenderFrame(0)
	, mPrepareIndexCounter(0)
	, mFinishedPrepareIndexCounter(0)
	, mRenderIndexCounter(0)
	, mPendingRenderIndexCounter(0)
	, mNumBuffersInFlight(0)
	, mIsPreparing(false)
	, mIsRendering(false)
	, mIsRenderingMainThread(false)
	, mPendingEndPrepare(false)
	, mIsReset(true)
	, mStartedRenderingFrame(false)
{
    mFramesInFlightSemaphore.Release(mNumBuffers);
}

SGfx_ViewDataBufferer::~SGfx_ViewDataBufferer() 
{
    WaitForStartRenderEvent();
    WaitForEndRenderEvent();
	FinishEndPrepare();
}

uint32 SGfx_ViewDataBufferer::GetNumBuffers() const
{
    return mNumBuffers;
}

uint32 SGfx_ViewDataBufferer::GetPrepareIndex() const
{ 
    return mPrepareIndex; 
}
uint32 SGfx_ViewDataBufferer::GetRenderIndex() const
{ 
    return mRenderIndex; 
}

void SGfx_ViewDataBufferer::StartPrepare()
{
    SC_PROFILER_FUNCTION();

    SC_ASSERT(SC_Thread::gIsMainThread);
    SC_ASSERT(!mIsPreparing);
    SC_ASSERT(mIsReset);

    FinishEndPrepare();

    if (!mStartedRenderingFrame)
        mFramesInFlightSemaphore.Release();

    if (!mFramesInFlightSemaphore.TimedAcquire(0))
        mFramesInFlightSemaphore.Acquire();

    mIsPreparing = true;
    mIsReset = false;
    mStartedRenderingFrame = false;
    ++mPrepareIndexCounter;
    mPrepareIndex = mPrepareIndexCounter % mNumBuffers; 
	mPendingRenderIndexCounter = mPrepareIndexCounter - mNumBuffersInFlight;
    mNumBuffersInFlight = SC_Min(mNumBuffersInFlight + 1, uint32(mNumBuffers - 2));
}

void SGfx_ViewDataBufferer::EndPrepare()
{
    if (!mIsPreparing)
		return;

	SC_PROFILER_FUNCTION();

	SC_ASSERT(SC_Thread::gIsMainThread);
	SC_ASSERT(!mPendingEndPrepare);

	SC_MutexLock lock(mRenderMutex);
	if (mRenderIndexCounter < mPendingRenderIndexCounter)
	    mFinishedPrepareIndexCounter = mPrepareIndexCounter;
	else
		mPendingEndPrepare = true;

    mIsPreparing = false;
}

void SGfx_ViewDataBufferer::ResetPrepare()
{
	SC_PROFILER_FUNCTION();
	SC_ASSERT(SC_Thread::gIsMainThread);
	SC_ASSERT(!mIsPreparing);
	FinishEndPrepare();
	mIsReset = true;
}

void SGfx_ViewDataBufferer::StartRender()
{
	SC_PROFILER_FUNCTION();
	SC_ASSERT(SC_Thread::gIsMainThread);
    SC_ASSERT(!mIsRenderingMainThread);
    SC_ASSERT(!mStartedRenderingFrame);

    mIsRenderingMainThread = true;
    mStartedRenderingFrame = true;
    WaitForStartRenderEvent();

    uint32 renderIndexCounter = mPendingRenderIndexCounter;

	mStartRenderEvent = SR_RenderDevice::gInstance->PostGraphicsTask([this, renderIndexCounter]() {
		SC_PROFILER_FUNCTION();
        SC_MutexLock lock(mRenderMutex);
        mRenderIndexCounter = renderIndexCounter;
        mRenderIndex = renderIndexCounter % mNumBuffers;
        mIsRendering = true;
    });
}

void SGfx_ViewDataBufferer::EndRender()
{
	SC_ASSERT(SC_Thread::gIsMainThread);
	if (!mIsRenderingMainThread)
		return;

	SC_PROFILER_FUNCTION();
    WaitForEndRenderEvent();

	mEndRenderEvent = SR_RenderDevice::gInstance->PostGraphicsTask([this]() {
		SC_PROFILER_FUNCTION();
        SC_ASSERT(mIsRendering);
		mIsRendering = false;
        mFramesInFlightSemaphore.Release();
		});

    mIsRenderingMainThread = false;
}

void SGfx_ViewDataBufferer::WaitForPrepareTask(const SC_Future<bool>& aTaskEvent)
{
    SyncWithRenderThread();
    if (mRenderIndexCounter > mFinishedPrepareIndexCounter)
    {
		SC_ReadLock<SC_ReadWriteMutex> lock(mWaitingForPrepareMutex);
        if (mRenderIndexCounter > mFinishedPrepareIndexCounter)
        {
            aTaskEvent.Wait();
            lock.Unlock();
            SyncWithRenderThread();
        }
    }
}

void SGfx_ViewDataBufferer::FinishEndPrepare()
{
    SC_ASSERT(SC_Thread::gIsMainThread);
    {
        SC_WriteLock<SC_ReadWriteMutex> lock(mWaitingForPrepareMutex);
        if (mPendingEndPrepare)
        {
            mFinishedPrepareIndexCounter = mPrepareIndexCounter;
            mPendingEndPrepare = false;
        }
    }
    SC_ASSERT(!mPendingEndPrepare);
}

void SGfx_ViewDataBufferer::SyncWithRenderThread()
{
    if (mPendingEndPrepare)
	{
		SC_WriteLock<SC_ReadWriteMutex> lock(mWaitingForPrepareMutex);
        if (mPendingEndPrepare)
        {
            mFinishedPrepareIndexCounter = mPrepareIndexCounter;
            mPendingEndPrepare = false;
        }
    }
}

void SGfx_ViewDataBufferer::WaitForStartRenderEvent()
{
    if (mStartRenderEvent)
    {
        if (!mStartRenderEvent->mCPUEvent.IsSignalled())
            mStartRenderEvent->mCPUEvent.Wait();

        mStartRenderEvent.Reset();
    }
}

void SGfx_ViewDataBufferer::WaitForEndRenderEvent()
{
	if (mEndRenderEvent)
	{
		if (!mEndRenderEvent->mCPUEvent.IsSignalled())
            mEndRenderEvent->mCPUEvent.Wait();

        mEndRenderEvent.Reset();
	}
}

SGfx_View::SGfx_View()
    : mOnlyDepth(false)
    , mIsMainView(false)
{
    mViewData.Respace(mViewDataBufferer.GetNumBuffers());
}

SGfx_View::~SGfx_View()
{
}

SGfx_ViewData& SGfx_View::GetPrepareData()
{
    return mViewData[mViewDataBufferer.GetPrepareIndex()];
}

const SGfx_ViewData& SGfx_View::GetRenderData()
{
	return mViewData[mViewDataBufferer.GetRenderIndex()];
}

SGfx_ViewData& SGfx_View::GetMutableRenderData()
{
	return mViewData[mViewDataBufferer.GetRenderIndex()];
}

void SGfx_View::SetCamera(const SGfx_Camera& aCamera)
{
    mCamera = aCamera;
}

const SGfx_Camera& SGfx_View::GetCamera() const
{
    return mCamera;
}

void SGfx_View::SetOnlyDepth(bool aValue)
{
    mOnlyDepth = aValue;
}

bool SGfx_View::IsOnlyDepth() const
{
    return mOnlyDepth;
}

void SGfx_View::SetMainView(bool aValue)
{
    mIsMainView = aValue;
}

bool SGfx_View::IsMainView() const
{
    return mIsMainView;
}

void SGfx_View::SetOutputTexture(const SC_Ref<SR_Texture>& aTexture)
{
    mOutputTexture = aTexture;
}

SR_Texture* SGfx_View::GetOutputTexture() const
{
    return mOutputTexture;
}

void SGfx_View::UpdateRenderSettings()
{
    bool temporalAllowed = true;

	mRenderSettings.mRenderPrepass = true;
	mRenderSettings.mRenderOpaque = true;
	mRenderSettings.mRenderTranslucent = true;
	mRenderSettings.mRenderUI = true;
	mRenderSettings.mRenderPostEffects = true;
	mRenderSettings.mRenderMotionVectors = true;
	mRenderSettings.mRenderDecals = true;
	mRenderSettings.mRenderParticles = true;
	mRenderSettings.mRenderTerrain = true;
	mRenderSettings.mRenderWater = true;
	mRenderSettings.mRenderSky = true;
	mRenderSettings.mRenderSun = true;
	mRenderSettings.mRenderClouds = true;
	mRenderSettings.mRenderFog = true;
	mRenderSettings.mComputeScattering = true;
	mRenderSettings.mRenderDebugPrimitives = true;
	mRenderSettings.mEnableShadowMaps = false;
	mRenderSettings.mEnableCascadedShadowMaps = false;
	mRenderSettings.mEnableDynamicShadowCascades = false;
	mRenderSettings.mEnableFarShadows = false;
	mRenderSettings.mEnableTAA &= temporalAllowed && mRenderSettings.mRenderPostEffects && SGfx_PostEffects::gEnableTAA;
	mRenderSettings.mEnableRoughnessAA = true;
	mRenderSettings.mEnableRTAO = true;
	mRenderSettings.mEnableRTGI = true;
	mRenderSettings.mEnableRaytracedLocalShadows = true;
	mRenderSettings.mEnableRaytracedSunShadows = true;
	mRenderSettings.mEnableRaytracedReflections = true;
	mRenderSettings.mEnableRaytracedRefractions = true;
}

SGfx_ViewRenderSettings& SGfx_View::GetRenderSettings()
{
    return mRenderSettings;
}

const SGfx_ViewRenderSettings& SGfx_View::GetRenderSettings() const
{
	return mRenderSettings;
}
