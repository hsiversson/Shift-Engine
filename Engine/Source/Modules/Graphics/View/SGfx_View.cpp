#include "SGfx_View.h"
#include "RenderCore/RenderTasks/SR_RenderThread.h"

SGfx_ViewDataBufferer::SGfx_ViewDataBufferer(uint32 aNumBuffers)
	: mNumBuffers(aNumBuffers)
	, mPrepareIndex(0)
	, mRenderIndex(0)
	, mCurrentStartedPrepareFrame(0)
	, mCurrentStartedRenderFrame(0)
	, mPrepareIndexCounter(0)
	, mRenderIndexCounter(0)
	, mPendingRenderIndexCounter(0)
	, mNumBuffersInFlight(0)
	, mIsPreparing(false)
	, mIsRendering(false)
{
    mStartRenderEvent.Signal();
    mEndRenderEvent.Signal();
}

SGfx_ViewDataBufferer::~SGfx_ViewDataBufferer() 
{
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
    SC_ASSERT(!mIsPreparing);

    mIsPreparing = true;
    ++mPrepareIndexCounter;
    mPrepareIndex = mPrepareIndexCounter % mNumBuffers; 
	mPendingRenderIndexCounter = mPrepareIndexCounter - mNumBuffersInFlight;
    mNumBuffersInFlight = SC_Min(mNumBuffersInFlight + 1, uint32(mNumBuffers - 2));
}

void SGfx_ViewDataBufferer::EndPrepare()
{
    if (!mIsPreparing)
        return;

	//SC_MutexLock lock(mRenderMutex);
	//if (mRenderIndexCounter < mPrepareIndexCounter)
	//{
	//    mfinisedPrepatre = mPrepareIndexCounter;
	//}
	//else
	//    asd = true;

    mIsPreparing = false;
}

void SGfx_ViewDataBufferer::StartRender()
{
	mStartRenderEvent.Wait();
    mStartRenderEvent.Reset();

    uint32 renderIndexCounter = mPrepareIndexCounter;
    SR_RenderThread::Get()->PostTask([this, renderIndexCounter]() {
        SC_MutexLock lock(mRenderMutex);
        mRenderIndexCounter = renderIndexCounter;
        mRenderIndex = mRenderIndexCounter % mNumBuffers;
        mIsRendering = true;
        mStartRenderEvent.Signal();
    });
}

void SGfx_ViewDataBufferer::EndRender()
{
	mEndRenderEvent.Wait();
	mEndRenderEvent.Reset();

    SR_RenderThread::Get()->PostTask([this]() {
		mIsRendering = false;
		mEndRenderEvent.Signal();
    });
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
