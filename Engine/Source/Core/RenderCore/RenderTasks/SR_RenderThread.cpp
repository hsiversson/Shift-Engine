#include "SR_RenderThread.h"

thread_local SR_CommandListType SR_RenderThread::gCurrentContextType = SR_CommandListType::Unknown;
thread_local SC_Ref<SR_CommandList> SR_RenderThread::gCurrentCommandList = nullptr;

SR_RenderThread::SR_RenderThread(const SR_CommandListType& aContextType)
	: mContextType(aContextType)
{
	switch (aContextType)
	{
	case SR_CommandListType::Graphics:
		SetName("Graphics Thread");
		break;
	case SR_CommandListType::Compute:
		SetName("Compute Thread");
		break;
	case SR_CommandListType::Copy:
		SetName("Copy Thread");
		break;
	default:
		SetName("Unknown Queue Thread");
		break;
	}
	Start();
}

SR_RenderThread::~SR_RenderThread()
{
	Stop(true);
}

SC_Ref<SR_TaskEvent> SR_RenderThread::PostTask(SR_RenderTaskSignature aTask)
{
	SC_Ref<SR_TaskEvent> taskEvent = SC_MakeRef<SR_TaskEvent>();

	auto taskFn = [this, aTask]()
	{
		SR_CommandListType prevContextType = gCurrentContextType;
		SC_Ref<SR_CommandList> prevCmdList = gCurrentCommandList;
		gCurrentContextType = mContextType;
		gCurrentCommandList = nullptr;
		aTask();
		SC_Ref<SR_CommandList> usedCmdList = gCurrentCommandList;
		gCurrentContextType = prevContextType;
		gCurrentCommandList = prevCmdList;
		return usedCmdList;
	};

	Task task;
	task.mTask = taskFn;
	task.mTaskEvent = taskEvent;

	SC_MutexLock lock(mTaskQueueMutex);
	mTaskQueue.Add(task);
	mHasWorkEvent.Signal();
	return taskEvent;
}

void SR_RenderThread::ThreadMain()
{
	SC_Thread::gIsRenderThread = (mContextType == SR_CommandListType::Graphics);

	while (mIsRunning)
	{
		SC_PROFILER_FUNCTION();
		mHasWorkEvent.Wait();
		mHasWorkEvent.Reset();

		for (;;)
		{
			Task task;
			{
				SC_MutexLock lock(mTaskQueueMutex);
				if (mTaskQueue.IsEmpty())
				{
					lock.Unlock();
					break;
				}

				task = mTaskQueue.Peek();
				mTaskQueue.Remove();
			}

			SC_Ref<SR_CommandList> cmdList = task.mTask();
			if (cmdList)
			{
				cmdList->End();
				task.mTaskEvent->mFence = SR_RenderDevice::gInstance->GetQueueManager()->GetNextFence(mContextType);
				SR_RenderDevice::gInstance->GetQueueManager()->SubmitCommandLists(mContextType, task.mTaskEvent->mFence, { cmdList });
				task.mTaskEvent->mCPUEvent.Signal();
			}
			else
				task.mTaskEvent->mCPUEvent.Signal();
		}
	}
}