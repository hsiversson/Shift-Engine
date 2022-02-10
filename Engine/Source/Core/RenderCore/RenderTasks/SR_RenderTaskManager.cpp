#include "SR_RenderTaskManager.h"

thread_local SR_CommandListType SR_RenderTaskManager::gCurrentTaskType = SR_CommandListType::Unknown;
thread_local SC_Ref<SR_CommandList> SR_RenderTaskManager::gCurrentCommandList = nullptr;

SR_RenderTaskManager::SR_RenderTaskManager()
{
}

SR_RenderTaskManager::~SR_RenderTaskManager()
{

}

bool SR_RenderTaskManager::Init()
{
	for (uint32 i = 0; i < static_cast<uint32>(SR_CommandListType::COUNT); ++i)
	{
		mCommandQueueThreads[i] = SC_MakeUnique<CommandQueueThread>(static_cast<SR_CommandListType>(i), mFreeCommandLists[i], mFreeCmdListsMutex[i]);
		mCommandQueueThreads[i]->Start();
	}

	return true;
}

void SR_RenderTaskManager::SubmitTask(SR_RenderTaskFunctionSignature aTask, const SR_CommandListType& aCommandListType, SR_TaskEvent* aOutEvent)
{
	SC_ASSERT(aOutEvent, "Needs an event to function properly.");

	auto task = [this, aTask, aCommandListType]()
	{
		gCurrentTaskType = aCommandListType;
		gCurrentCommandList = nullptr;
		aTask();
		return GrabCurrentCommandList();
	};

	mCommandQueueThreads[static_cast<uint32>(aCommandListType)]->SubmitTask(task, aOutEvent);
}

void SR_RenderTaskManager::SubmitSplit(const SR_CommandListType& aCommandListType)
{
	mCommandQueueThreads[static_cast<uint32>(aCommandListType)]->SubmitSplit();
}

SC_Ref<SR_CommandList> SR_RenderTaskManager::GetCommandList(const SR_CommandListType& aCommandListType)
{
	if (aCommandListType == SR_CommandListType::Unknown)
		return nullptr;

	if (gCurrentCommandList)
		return gCurrentCommandList;

	std::queue<SC_Ref<SR_CommandList>>& freeCmdLists = mFreeCommandLists[static_cast<uint32>(aCommandListType)];

	SC_Ref<SR_CommandList> cmdList;
	{
		SC_MutexLock lock(mFreeCmdListsMutex[static_cast<uint32>(aCommandListType)]);
		if (!freeCmdLists.empty())
		{
			cmdList = freeCmdLists.front();
			freeCmdLists.pop();
		}
	}

	if (!cmdList)
	{
		cmdList = SR_RenderDevice::gInstance->CreateCommandList(aCommandListType);
	}

	cmdList->Begin();
	gCurrentCommandList = cmdList;
	return cmdList;
}

SC_Ref<SR_CommandList> SR_RenderTaskManager::GrabCurrentCommandList()
{
	return gCurrentCommandList;
}

SR_RenderTaskManager::CommandQueueThread::CommandQueueThread(const SR_CommandListType& aCommandListType, std::queue<SC_Ref<SR_CommandList>>& aFreeCommandLists, SC_Mutex& aFreeCmdListsMutex)
	: mCommandListType(aCommandListType)
	, mFreeCommandLists(aFreeCommandLists)
	, mFreeCmdListsMutex(aFreeCmdListsMutex)
	, mLatestPushedTaskWasSplit(false)
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

}

SR_RenderTaskManager::CommandQueueThread::~CommandQueueThread()
{
	Stop(false);
	mHasWorkEvent.Signal();
	Wait();
}

void SR_RenderTaskManager::CommandQueueThread::ThreadMain()
{
	SC_Array<InFlightTask> inFlightTasksKeepAlive;
	SC_Array<SR_CommandList*> cmdListsReadyForExecute;

	SC_Array<SC_Ref<SR_CommandList>> cmdListsToReturn;
	cmdListsToReturn.Reserve(64);

	std::queue<InFlightTask> localInFlightTasks;
	while (mIsRunning)
	{
		if (localInFlightTasks.empty() && mInFlightCommandLists.empty())
		{
			mHasWorkEvent.Wait();
			mHasWorkEvent.Reset();
		}

		//if (!mInFlightTasks.empty())
		{
			SC_MutexLock lock(mInFlightTasksMutex);
			while (!mInFlightTasks.empty())
			{
				localInFlightTasks.push(SC_Move(mInFlightTasks.front()));
				mInFlightTasks.pop();
			}
		}

		cmdListsReadyForExecute.RemoveAll();
		inFlightTasksKeepAlive.RemoveAll();

		while (!localInFlightTasks.empty())
		{
			InFlightTask& inFlightTask = localInFlightTasks.front();
			if (inFlightTask.mIsSplit)
			{
				localInFlightTasks.pop();
				if (cmdListsReadyForExecute.IsEmpty()) // No need to exit loop if the split was our first task.
					continue;
				else
					break;
			}

			if (inFlightTask.mFuture.IsReady())
			{
				SC_Ref<SR_CommandList> cmdList = inFlightTask.mFuture.GetValue();
				if (cmdList)
				{
					cmdList->End();
					inFlightTasksKeepAlive.Add(inFlightTask);
					cmdListsReadyForExecute.Add(cmdList.get());
				}
				else
					inFlightTask.mEvent->mCPUEvent.Signal();

				localInFlightTasks.pop();
			}
			else
				break;
		}
		if (!cmdListsReadyForExecute.IsEmpty())
		{
			SR_Fence executeFence = SR_RenderDevice::gInstance->GetCommandQueue(mCommandListType)->SubmitCommandLists(cmdListsReadyForExecute.GetBuffer(), cmdListsReadyForExecute.Count());

			for (InFlightTask& task : inFlightTasksKeepAlive)
			{
				task.mEvent->mFence = executeFence;
				task.mEvent->mCPUEvent.Signal();
				mInFlightCommandLists.push(SC_Pair(task.mFuture.GetValue(), task.mEvent->mFence));
			}
		}

		if (localInFlightTasks.empty())
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

void SR_RenderTaskManager::CommandQueueThread::SubmitTask(InFlightTaskSignature aTask, SR_TaskEvent* aOutEvent)
{
	InFlightTask inFlightTask;
	inFlightTask.mEvent = aOutEvent;
	inFlightTask.mFuture = SC_ThreadPool::Get().SubmitTask(aTask);
	inFlightTask.mIsSplit = false;

	mLatestPushedTaskWasSplit = false;

	SC_MutexLock lock(mInFlightTasksMutex);
	mInFlightTasks.push(inFlightTask);
	mHasWorkEvent.Signal();
}

void SR_RenderTaskManager::CommandQueueThread::SubmitSplit()
{
	if (mLatestPushedTaskWasSplit)
		return;

	mLatestPushedTaskWasSplit = true;

	InFlightTask inFlightTask;
	inFlightTask.mIsSplit = true;

	SC_MutexLock lock(mInFlightTasksMutex);
	mInFlightTasks.push(inFlightTask);
	mHasWorkEvent.Signal();
}
