#include "SR_CommandQueueManager.h"
#if 0
thread_local SR_CommandListType SR_CommandQueueManager::gCurrentTaskType = SR_CommandListType::Unknown;
thread_local SC_Ref<SR_CommandList> SR_CommandQueueManager::gCurrentCommandList = nullptr;

SR_CommandQueueManager::SR_CommandQueueManager()
{
}

SR_CommandQueueManager::~SR_CommandQueueManager()
{

}

bool SR_CommandQueueManager::Init()
{
	for (uint32 i = 0; i < static_cast<uint32>(SR_CommandListType::COUNT); ++i)
	{
		mCommandSubmissionThreads[i] = SC_MakeUnique<CommandSubmissionThread>(static_cast<SR_CommandListType>(i));
		mCommandQueueThreads[i] = SC_MakeUnique<CommandQueueThread>(static_cast<SR_CommandListType>(i), mCommandSubmissionThreads[i].get());

		mCommandSubmissionThreads[i]->Start();
		mCommandQueueThreads[i]->Start();
	}

	return true;
}

void SR_CommandQueueManager::SubmitTask(SR_RenderTaskFunctionSignature aTask, const SR_CommandListType& aCommandListType, SR_TaskEvent* aOutEvent)
{
	SC_ASSERT(aOutEvent, "Needs an event to function properly.");

	auto task = [this, aTask, aCommandListType]()
	{
		SR_CommandListType prevCmdListType = gCurrentTaskType;
		SC_Ref<SR_CommandList> prevCmdList = gCurrentCommandList;
		gCurrentTaskType = aCommandListType;
		gCurrentCommandList = nullptr;
		aTask();
		SC_Ref<SR_CommandList> usedCmdList = GrabCurrentCommandList();
		gCurrentTaskType = prevCmdListType;
		gCurrentCommandList = prevCmdList;
		return usedCmdList;
	};

	mCommandQueueThreads[static_cast<uint32>(aCommandListType)]->SubmitTask(task, aOutEvent);
}

SC_Ref<SC_Event> SR_CommandQueueManager::SubmitLightTask(SR_RenderTaskFunctionSignature aTask, const SR_CommandListType& aCommandListType)
{
	SC_Ref<SC_Event> event = SC_MakeRef<SC_Event>();
	mCommandQueueThreads[static_cast<uint32>(aCommandListType)]->SubmitLightTask(aTask, event);
	return event;
}

SR_Fence SR_CommandQueueManager::InsertFence(const SR_CommandListType& aCommandListType)
{
	return mCommandQueueThreads[static_cast<uint32>(aCommandListType)]->InsertFence();
}

void SR_CommandQueueManager::InsertWait(const SR_Fence& aFence, const SR_CommandListType& aWaitingQueueType)
{
	return mCommandQueueThreads[static_cast<uint32>(aWaitingQueueType)]->InsertWait(aFence);
}

bool SR_CommandQueueManager::IsFencePending(const SR_Fence& aFence)
{
	if (aFence.mType == SR_CommandListType::Unknown)
	{
		SC_ASSERT(false, "Invalid fence.");
		return false;
	}

	return mCommandSubmissionThreads[static_cast<uint32>(aFence.mType)]->GetCommandQueue()->IsFencePending(aFence);
}

bool SR_CommandQueueManager::WaitForFence(const SR_Fence& aFence, bool aBlock)
{
	if (aFence.mType == SR_CommandListType::Unknown)
	{
		SC_ASSERT(false, "Invalid fence.");
		return false;
	}

	return mCommandSubmissionThreads[static_cast<uint32>(aFence.mType)]->GetCommandQueue()->WaitForFence(aFence, aBlock);
}

SC_Ref<SR_CommandList> SR_CommandQueueManager::GetCommandList(const SR_CommandListType& aCommandListType)
{
	if (aCommandListType == SR_CommandListType::Unknown)
		return nullptr;

	return mCommandSubmissionThreads[static_cast<uint32>(aCommandListType)]->GetCommandList();
}

SC_Ref<SR_CommandList> SR_CommandQueueManager::GrabCurrentCommandList()
{
	return gCurrentCommandList;
}

SR_CommandQueueManager::CommandQueueThread::CommandQueueThread(const SR_CommandListType& aCommandListType, CommandSubmissionThread* aCommandSubmissionThread)
	: mCommandListType(aCommandListType)
	, mCommandSubmissionThread(aCommandSubmissionThread)
{
	switch (aCommandListType)
	{
	case SR_CommandListType::Graphics:
		SetName("Graphics Queue Thread");
		break;
	case SR_CommandListType::Compute:
		SetName("Compute Queue Thread");
		break;
	case SR_CommandListType::Copy:
		SetName("Copy Queue Thread");
		break;
	default:
		SetName("Unknown Queue Thread");
		break;
	}

	mCommandQueue = SR_RenderDevice::gInstance->GetCommandQueue(mCommandListType);
}

SR_CommandQueueManager::CommandQueueThread::~CommandQueueThread()
{
	Stop(false);
	mHasWorkEvent.Signal();
	Wait();
}

void SR_CommandQueueManager::CommandQueueThread::ThreadMain()
{
	std::queue<CommandQueueWork> localWorkQueue;
	while (mIsRunning)
	{
		if (localWorkQueue.empty())
		{
			mHasWorkEvent.Wait();
			mHasWorkEvent.Reset();
		}

		{
			SC_MutexLock lock(mWorkQueueMutex);
			while (!mWorkQueue.empty())
			{
				localWorkQueue.push(SC_Move(mWorkQueue.front()));
				mWorkQueue.pop();
			}
		}

		while (!localWorkQueue.empty())
		{
			CommandQueueWork& work = localWorkQueue.front();
			switch (work.mType)
			{
			case CommandQueueWork::Type::Task:
			{
				SC_Future<SC_Ref<SR_CommandList>> future;
				if (work.mImmediateExecute)
				{
					SC_Ref<SC_Promise<SC_Ref<SR_CommandList>>> promise = SC_MakeRef<SC_Promise<SC_Ref<SR_CommandList>>>();
					future = promise->GetFuture(); 
					promise->SetValue(work.mRenderTask());
				}
				else
					future = SC_ThreadPool::Get().SubmitTask(work.mRenderTask);

				ScheduleSubmit(future, work.mRenderTaskEvent);
				
				break;
			}
			case CommandQueueWork::Type::LightTask:
			{
				if (work.mImmediateExecute)
				{
					work.mLightTask();
					work.mLightTaskEvent->Signal();
				}
				else
				{
					LightTaskSignature lightTask = work.mLightTask;
					SC_Ref<SC_Event> lightTaskEvent = work.mLightTaskEvent;
					auto task = [lightTask, lightTaskEvent]()
					{
						lightTask();
						lightTaskEvent->Signal();
					};
					SC_ThreadPool::Get().SubmitTask(task);
				}
				break;
			}
			case CommandQueueWork::Type::SignalFence:
			{
				mCommandSubmissionThread->InsertFence(work.mFence);
				break;
			}
			case CommandQueueWork::Type::WaitForFence:
			{
				mCommandSubmissionThread->InsertWait(work.mFence);
				break;
			}
			}

			localWorkQueue.pop();
		}
	}
}

void SR_CommandQueueManager::CommandQueueThread::SubmitTask(RenderTaskSignature aTask, SR_TaskEvent* aOutEvent)
{
	CommandQueueWork work;
	work.mType = CommandQueueWork::Type::Task;
	work.mRenderTaskEvent = aOutEvent;
	work.mRenderTask = aTask;
	work.mImmediateExecute = true;

	SC_MutexLock lock(mWorkQueueMutex);
	mWorkQueue.push(work);
	mHasWorkEvent.Signal();
}

void SR_CommandQueueManager::CommandQueueThread::SubmitLightTask(LightTaskSignature aTask, SC_Ref<SC_Event>& aOutEvent)
{
	CommandQueueWork work;
	work.mType = CommandQueueWork::Type::LightTask;
	work.mLightTaskEvent = aOutEvent;
	work.mLightTask = aTask;
	work.mImmediateExecute = true;

	SC_MutexLock lock(mWorkQueueMutex);
	mWorkQueue.push(work);
	mHasWorkEvent.Signal();
}

SR_Fence SR_CommandQueueManager::CommandQueueThread::InsertFence()
{
	CommandQueueWork work;
	work.mFence = mCommandQueue->GetNextFence();
	work.mType = CommandQueueWork::Type::SignalFence;
	if (SC_Thread::gCurrentThread == this)
	{
		mCommandQueue->SignalFence(work.mFence);
	}
	else
	{
		SC_MutexLock lock(mWorkQueueMutex);
		mWorkQueue.push(work);
		mHasWorkEvent.Signal();
	}
	return work.mFence;
}

void SR_CommandQueueManager::CommandQueueThread::InsertWait(const SR_Fence& aFence)
{
	if (aFence.mType == mCommandListType) // A queue never has to wait on a fence from itself
		return;

	CommandQueueWork work;
	work.mType = CommandQueueWork::Type::WaitForFence;
	work.mFence = aFence;

	SC_MutexLock lock(mWorkQueueMutex);
	mWorkQueue.push(work);
	mHasWorkEvent.Signal();
}

void SR_CommandQueueManager::CommandQueueThread::ScheduleSubmit(SC_Future<SC_Ref<SR_CommandList>> aFuture, SR_TaskEvent* aEvent)
{
	mCommandSubmissionThread->SubmitTask(aFuture, aEvent);
}

SR_CommandQueueManager::CommandSubmissionThread::CommandSubmissionThread(const SR_CommandListType& aCommandListType)
	: mCommandListType(aCommandListType)
{
	switch (aCommandListType)
	{
	case SR_CommandListType::Graphics:
		SetName("Graphics Submission Thread");
		break;
	case SR_CommandListType::Compute:
		SetName("Compute Submission Thread");
		break;
	case SR_CommandListType::Copy:
		SetName("Copy Submission Thread");
		break;
	default:
		SetName("Unknown Submission Thread");
		break;
	}

	mCommandQueue = SR_RenderDevice::gInstance->GetCommandQueue(mCommandListType);
}

SR_CommandQueueManager::CommandSubmissionThread::~CommandSubmissionThread()
{

}

void SR_CommandQueueManager::CommandSubmissionThread::ThreadMain()
{
	SC_Array<SR_TaskEvent*> eventsToSignal(16);
	SC_Array<SC_Ref<SR_CommandList>> cmdListsToSubmit(16);
	SC_Array<SC_Ref<SR_CommandList>> cmdListsToReturn(16);
	SC_Queue<Work> localWorkQueue;
	while (mIsRunning)
	{
		if (localWorkQueue.empty() && mInFlightCommandLists.empty())
		{
			mHasWorkEvent.Wait();
			mHasWorkEvent.Reset();
		}

		{
			SC_MutexLock lock(mWorkQueueMutex);
			while (!mWorkQueue.empty())
			{
				localWorkQueue.push(SC_Move(mWorkQueue.front()));
				mWorkQueue.pop();
			}
		}

		bool shouldContinue = true;
		while (shouldContinue && !localWorkQueue.empty())
		{
			Work& work = localWorkQueue.front();
			switch (work.mType)
			{
			case WorkType::Task:
				if (work.mTask.mFuture.IsReady())
				{
					if (SC_Ref<SR_CommandList> cmdList = work.mTask.mFuture.GetValue())
					{
						cmdList->End();
						cmdListsToSubmit.Add(cmdList);// We can schedule for GPU submission
						eventsToSignal.Add(work.mTask.mEvent);
					}
					else
						work.mTask.mEvent->mCPUEvent.Signal(); // No commands were recorded for GPU, signal that CPU task is ready.
				}
				else
					shouldContinue = false;

				break;
			case WorkType::Signal:
				SubmitCommandLists(cmdListsToSubmit, eventsToSignal);
				mCommandQueue->SignalFence(work.mFence);
				break;
			case WorkType::Wait:
				SubmitCommandLists(cmdListsToSubmit, eventsToSignal);
				mCommandQueue->WaitForFence(work.mFence);
				break;
			}

			if (shouldContinue)
				localWorkQueue.pop();
		}
		SubmitCommandLists(cmdListsToSubmit, eventsToSignal);

		while (!mInFlightCommandLists.empty())
		{
			SC_Pair<SC_Ref<SR_CommandList>, SR_Fence>& cmdList = mInFlightCommandLists.front();
			if (!cmdList.mSecond.IsPending())
			{
				cmdListsToReturn.Add(cmdList.mFirst);
				mInFlightCommandLists.pop();
			}
			else
				break;
		}

		if (!cmdListsToReturn.IsEmpty())
		{
			{
				SC_MutexLock lock(mFreeCmdListsMutex);
				for (const SC_Ref<SR_CommandList>& cmdList : cmdListsToReturn)
					mFreeCommandLists.push(cmdList);
			}
			cmdListsToReturn.RemoveAll();
		}
	}
}

void SR_CommandQueueManager::CommandSubmissionThread::SubmitTask(SC_Future<SC_Ref<SR_CommandList>> aFuture, SR_TaskEvent* aEvent)
{
	Work work;
	work.mTask.mFuture = aFuture;
	work.mTask.mEvent = aEvent;
	work.mType = WorkType::Task;

	SC_MutexLock lock(mWorkQueueMutex);
	mWorkQueue.push(work);
	mHasWorkEvent.Signal();
}

void SR_CommandQueueManager::CommandSubmissionThread::InsertFence(const SR_Fence& aFence)
{
	Work work;
	work.mFence = aFence;
	work.mType = WorkType::Signal;

	SC_MutexLock lock(mWorkQueueMutex);
	mWorkQueue.push(work);
	mHasWorkEvent.Signal();
}

void SR_CommandQueueManager::CommandSubmissionThread::InsertWait(const SR_Fence& aFence)
{
	Work work;
	work.mFence = aFence;
	work.mType = WorkType::Wait;

	SC_MutexLock lock(mWorkQueueMutex);
	mWorkQueue.push(work);
	mHasWorkEvent.Signal();
}

SR_CommandQueue* SR_CommandQueueManager::CommandSubmissionThread::GetCommandQueue()
{
	return mCommandQueue;
}

SC_Ref<SR_CommandList> SR_CommandQueueManager::CommandSubmissionThread::GetCommandList()
{
	if (gCurrentCommandList)
		return gCurrentCommandList;

	std::queue<SC_Ref<SR_CommandList>>& freeCmdLists = mFreeCommandLists;

	SC_Ref<SR_CommandList> cmdList;
	{
		SC_MutexLock lock(mFreeCmdListsMutex);
		if (!freeCmdLists.empty())
		{
			cmdList = freeCmdLists.front();
			freeCmdLists.pop();
		}
	}

	if (!cmdList)
	{
		cmdList = SR_RenderDevice::gInstance->CreateCommandList(mCommandListType);
	}

	cmdList->Begin();
	gCurrentCommandList = cmdList;
	return cmdList;
}

void SR_CommandQueueManager::CommandSubmissionThread::SubmitCommandLists(SC_Array<SC_Ref<SR_CommandList>>& aCommandLists, SC_Array<SR_TaskEvent*>& aEventsToSignal)
{
	if (!aCommandLists.IsEmpty())
	{
		SC_Array<SR_CommandList*> cmdLists(aCommandLists.Count());
		for (auto& cmdList : aCommandLists)
			cmdLists.Add(cmdList);

		mCommandQueue->SubmitCommandLists(cmdLists.GetBuffer(), cmdLists.Count());
		SR_Fence executionFence = mCommandQueue->GetNextFence();
		mCommandQueue->SignalFence(executionFence);

		uint32 index = 0;
		for (SR_TaskEvent* taskEvent : aEventsToSignal)
		{
			taskEvent->mFence = executionFence;
			mInFlightCommandLists.push(SC_Pair(aCommandLists[index++], taskEvent->mFence));
			taskEvent->mCPUEvent.Signal();
		}
		aCommandLists.RemoveAll();
		aEventsToSignal.RemoveAll();
	}
}
#endif