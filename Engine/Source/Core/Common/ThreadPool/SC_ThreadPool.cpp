#include "SC_ThreadPool.h"

SC_ThreadPool* SC_ThreadPool::gInstance = nullptr;

bool SC_ThreadPool::Create()
{
    if (!gInstance)
    {
		uint32 numSystemThreads = std::thread::hardware_concurrency();
		uint32 numPoolThreads = (numSystemThreads > 8) ? (numSystemThreads - 2) : numSystemThreads;
		gInstance = new SC_ThreadPool(numPoolThreads);
        return true;
    }
    return false;
}

void SC_ThreadPool::Destroy()
{
    delete gInstance;
    gInstance = nullptr;
}

SC_ThreadPool& SC_ThreadPool::Get()
{
    SC_ASSERT(gInstance != nullptr);
    return *gInstance;
}

uint32 SC_ThreadPool::GetTotalTaskCount() const
{
    return mTotalTaskCount;
}

uint32 SC_ThreadPool::GetQueuedTaskCount() const
{
	SC_MutexLock lock(mQueueMutex);
    return (uint32)mTasks.size();
}

uint32 SC_ThreadPool::GetRunningTaskCount() const
{
    return mTotalTaskCount - GetQueuedTaskCount();
}

uint32 SC_ThreadPool::GetThreadCount() const
{
    return mThreadCount;
}

bool SC_ThreadPool::IsPaused() const
{
    return mIsPaused;
}

void SC_ThreadPool::Pause()
{
    mIsPaused = true;
}

void SC_ThreadPool::Resume()
{
    mIsPaused = false;
}

SC_ThreadPool::SC_ThreadPool(uint32 aThreadCount)
    : mThreadCount(aThreadCount)
    , mIsPaused(false)
{
    mThreads = new WorkerThread[mThreadCount];
	for (uint32 i = 0; i < mThreadCount; i++)
	{
		std::string name = "Worker Thread ";
		name += std::to_string(i);
        WorkerThread& thread = mThreads[i];
        thread.SetThreadPool(this);
		thread.SetName(name.c_str());
		thread.Start();
	}
}

SC_ThreadPool::~SC_ThreadPool()
{
	for (uint32 i = 0; i < mThreadCount; i++)
        mThreads[i].Stop(false);

    mHasWorkEvent.Signal();

	for (uint32 i = 0; i < mThreadCount; i++)
		mThreads[i].Wait();

    delete[] mThreads;
}

bool SC_ThreadPool::PopTask(std::function<void()>& aOutTask)
{
    SC_MutexLock lock(mQueueMutex);
    if (!mTasks.empty())
    {
        aOutTask = std::move(mTasks.front());
        mTasks.pop();
        return true;
    }

    return false;
}

SC_ThreadPool::WorkerThread::WorkerThread()
    : mThreadPool(nullptr)
{
}

void SC_ThreadPool::WorkerThread::SetThreadPool(SC_ThreadPool* aThreadPool)
{
    mThreadPool = aThreadPool;
}

void SC_ThreadPool::WorkerThread::ThreadMain()
{
    SC_Thread::gIsTaskThread = true;

	while (mIsRunning)
	{
        mThreadPool->mHasWorkEvent.Wait();
        mThreadPool->mHasWorkEvent.Reset();

		SC_Function<void()> task;
		while (mThreadPool && (!mThreadPool->IsPaused() && mThreadPool->PopTask(task)))
		{
			task();
			--mThreadPool->mTotalTaskCount;
		}
	}
}
