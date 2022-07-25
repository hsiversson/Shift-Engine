#include "SR_RenderThread.h"

#if 0
SR_RenderThread* SR_RenderThread::gInstance = nullptr;

SR_RenderThread::SR_RenderThread()
	: mCurrentFrameIndex(0)
{
	SC_ASSERT(gInstance == nullptr, "Only one RenderThread may exist.");
	gInstance = this;

	SetName("Render Thread");
	Start();
}

SR_RenderThread::~SR_RenderThread()
{
	Stop(true);
	gInstance = nullptr;
}

void SR_RenderThread::PostTask(std::function<void()> aTask)
{
	SC_MutexLock lock(mTaskQueueMutex);
	mTaskQueue.push(aTask);
	mHasWorkEvent.Signal();
}

uint64 SR_RenderThread::GetCurrentFrameIndex() const
{
	return mCurrentFrameIndex;
}

void SR_RenderThread::Synchronize()
{
	// Insert waitable and wait for it.
	SC_ASSERT(!gIsRenderThread, "RenderThread should never sync with itself.");
	if (!gIsRenderThread)
	{
		mEndOfFrameEvent.Wait();
		mEndOfFrameEvent.Reset();
	}
}

void SR_RenderThread::EndFrame(uint32 aUpdateThreadFrameIdx)
{
	SC_ASSERT(gIsRenderThread);
	mCurrentFrameIndex = aUpdateThreadFrameIdx;
	mEndOfFrameEvent.Signal();
	SR_RenderDevice::gInstance->gLatestFinishedFrame = mCurrentFrameIndex;
}

SR_RenderThread* SR_RenderThread::Get()
{
	return gInstance;
}

void SR_RenderThread::ThreadMain()
{
	SC_Thread::gIsRenderThread = true;

	std::queue<std::function<void()>> tasks;
	while (mIsRunning)
	{
		SC_PROFILER_FUNCTION();
		mHasWorkEvent.Wait();
		mHasWorkEvent.Reset();

		{
			SC_MutexLock lock(mTaskQueueMutex);
			tasks.swap(mTaskQueue);
		}

		while (!tasks.empty())
		{
			tasks.front()();
			tasks.pop();
		}
	}
}
#endif