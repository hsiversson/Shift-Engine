#include "SR_QueueManager.h"

thread_local SR_CommandListType SR_QueueManager::gCurrentTaskType = SR_CommandListType::Unknown;
thread_local SC_Ref<SR_CommandList> SR_QueueManager::gCurrentCommandList = nullptr;

SR_QueueManager::SR_QueueManager()
{
}

SR_QueueManager::~SR_QueueManager()
{

}

bool SR_QueueManager::Init()
{
	for (uint32 i = 0; i < static_cast<uint32>(SR_CommandListType::COUNT); ++i)
	{
		mCommandQueueThreads[i] = SC_MakeUnique<CommandQueueThread>(static_cast<SR_CommandListType>(i));
		mCommandQueueThreads[i]->Start();
	}

	return true;
}

void SR_QueueManager::SubmitTask(SR_RenderTaskFunctionSignature aTask, const SR_CommandListType& aCommandListType, SR_TaskEvent* aOutEvent)
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

SR_Fence SR_QueueManager::InsertFence(const SR_CommandListType& aCommandListType)
{
	return mCommandQueueThreads[static_cast<uint32>(aCommandListType)]->InsertFence();
}

void SR_QueueManager::InsertFence(const SR_CommandListType& aCommandListType, SR_Fence& aFence)
{
	aFence.mType = aCommandListType;
	mCommandQueueThreads[static_cast<uint32>(aCommandListType)]->InsertFence(aFence);
}

void SR_QueueManager::InsertWait(const SR_Fence& /*aFence*/, const SR_CommandListType& /*aCommandListType*/)
{
}

SC_Ref<SR_CommandList> SR_QueueManager::GetCommandList(const SR_CommandListType& aCommandListType)
{
	if (aCommandListType == SR_CommandListType::Unknown)
		return nullptr;

	return mCommandQueueThreads[static_cast<uint32>(aCommandListType)]->GetCommandList();
}

SC_Ref<SR_CommandList> SR_QueueManager::GrabCurrentCommandList()
{
	return gCurrentCommandList;
}

SR_QueueManager::CommandQueueThread::CommandQueueThread(const SR_CommandListType& aCommandListType)
	: mCommandListType(aCommandListType)
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
	mCommandQueueFence = SR_RenderDevice::gInstance->CreateFenceResource();
}

SR_QueueManager::CommandQueueThread::~CommandQueueThread()
{
	Stop(false);
	mHasWorkEvent.Signal();
	Wait();
}

void SR_QueueManager::CommandQueueThread::ThreadMain()
{
	SC_Array<CommandQueueWork> workItemsToSignal;
	SC_Array<SR_CommandList*> cmdListsToSubmit;
	workItemsToSignal.Reserve(16);
	cmdListsToSubmit.Reserve(16);

	SC_Array<SC_Ref<SR_CommandList>> cmdListsToReturn;
	cmdListsToReturn.Reserve(64);

	std::queue<CommandQueueWork> localWorkQueue;
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

		while (!localWorkQueue.empty())
		{
			CommandQueueWork& work = localWorkQueue.front();
			if (work.mType == CommandQueueWork::Type::SignalFence || work.mType == CommandQueueWork::Type::WaitForFence)
			{
				// Flush pending gpu submissions & submit the signal/wait.
				SubmitCommandLists(cmdListsToSubmit, workItemsToSignal);

				if (work.mType == CommandQueueWork::Type::SignalFence)
					mCommandQueue->InsertFence(work.mFence);
				else
					mCommandQueue->InsertWait(work.mFence);
			}
			else if (work.mFuture.IsReady())
			{
				SC_Ref<SR_CommandList> cmdList = work.mFuture.GetValue();
				
				if (cmdList)
				{
					cmdList->End();
					cmdListsToSubmit.Add(cmdList);// We can schedule for GPU submission
					workItemsToSignal.Add(work);
				}
				else
					work.mEvent->mCPUEvent.Signal(); // No commands were recorded for GPU, signal that CPU task is ready.
			}
			else
			{
				break;
			}

			localWorkQueue.pop();
		}
		SubmitCommandLists(cmdListsToSubmit, workItemsToSignal);

		if (localWorkQueue.empty())
		{
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
}

void SR_QueueManager::CommandQueueThread::SubmitTask(InFlightTaskSignature aTask, SR_TaskEvent* aOutEvent)
{
	CommandQueueWork work;
	work.mType = CommandQueueWork::Type::Task;
	work.mEvent = aOutEvent;
	work.mFuture = SC_ThreadPool::Get().SubmitTask(aTask);

	SC_MutexLock lock(mWorkQueueMutex);
	mWorkQueue.push(work);
	mHasWorkEvent.Signal();
}

SR_Fence SR_QueueManager::CommandQueueThread::InsertFence()
{
	CommandQueueWork work;
	work.mType = CommandQueueWork::Type::SignalFence;
	work.mFence = mCommandQueueFence->GetNextFence();
	work.mFence.mType = mCommandListType;

	SC_MutexLock lock(mWorkQueueMutex);
	mWorkQueue.push(work);
	mHasWorkEvent.Signal();

	return work.mFence;
}

void SR_QueueManager::CommandQueueThread::InsertFence(const SR_Fence& aFence)
{
	CommandQueueWork work;
	work.mType = CommandQueueWork::Type::SignalFence;
	work.mFence = aFence;

	SC_MutexLock lock(mWorkQueueMutex);
	mWorkQueue.push(work);
	mHasWorkEvent.Signal();
}

void SR_QueueManager::CommandQueueThread::InsertWait(const SR_Fence& aFence)
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

SC_Ref<SR_CommandList> SR_QueueManager::CommandQueueThread::GetCommandList()
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

void SR_QueueManager::CommandQueueThread::SubmitCommandLists(SC_Array<SR_CommandList*>& aCommandLists, SC_Array<CommandQueueWork>& aWorkToSignal)
{
	if (!aCommandLists.IsEmpty())
	{
		mCommandQueue->SubmitCommandLists(aCommandLists.GetBuffer(), aCommandLists.Count());
		SR_Fence executionFence = mCommandQueueFence->GetNextFence();
		mCommandQueue->InsertFence(executionFence);
		for (CommandQueueWork& work : aWorkToSignal)
		{
			work.mEvent->mFence = executionFence;
			work.mEvent->mCPUEvent.Signal();
			mInFlightCommandLists.push(SC_Pair(work.mFuture.GetValue(), work.mEvent->mFence));
		}
		aCommandLists.RemoveAll();
		aWorkToSignal.RemoveAll();
	}
}
