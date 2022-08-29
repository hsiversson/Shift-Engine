#include "SR_QueueManager.h"

SR_QueueManager::SR_QueueManager()
{
}

SR_QueueManager::~SR_QueueManager()
{
	Action action;
	action.mType = ActionType::Quit;
	action.mQueueType = SR_CommandListType::Graphics;
	PushAction(action);
	Wait();
}

bool SR_QueueManager::Init()
{
	for (uint32 contextIndex = 0; contextIndex < static_cast<uint32>(SR_CommandListType::COUNT); ++contextIndex)
		mCommandQueueContexts[contextIndex].mCommandQueue = SR_RenderDevice::gInstance->GetCommandQueue(static_cast<SR_CommandListType>(contextIndex));

	SetName("Queue Manager Thread");
	Start();
	return true;
}

void SR_QueueManager::SubmitCommandLists(const SR_CommandListType& aQueueType, const SR_Fence& aFence, const SC_Array<SC_Ref<SR_CommandList>>& aCommandLists)
{
	Action action;
	action.mType = ActionType::SubmitCommandLists;
	action.mQueueType = aQueueType;
	action.mCommandLists = aCommandLists;
	action.mFence = aFence;
	PushAction(action);
}

void SR_QueueManager::SignalFence(const SR_CommandListType& aQueueType, const SR_Fence& aFence)
{
	Action action;
	action.mType = ActionType::SignalFence;
	action.mQueueType = aQueueType;
	action.mFence = aFence;
	PushAction(action);
}

void SR_QueueManager::InsertFence(const SR_CommandListType& aQueueType, const SR_Fence& aFence)
{
	Action action;
	action.mType = ActionType::InsertWait;
	action.mQueueType = aQueueType;
	action.mFence = aFence;
	PushAction(action);
}

bool SR_QueueManager::IsFencePending(const SR_Fence& aFence) const
{
	return mCommandQueueContexts[static_cast<uint32>(aFence.mType)].mCommandQueue->IsFencePending(aFence);
}

bool SR_QueueManager::WaitForFence(const SR_Fence& aFence, bool aBlock) const
{
	return mCommandQueueContexts[static_cast<uint32>(aFence.mType)].mCommandQueue->WaitForFence(aFence, aBlock);
}

SR_Fence SR_QueueManager::GetNextFence(const SR_CommandListType& aQueueType) const
{
	return mCommandQueueContexts[static_cast<uint32>(aQueueType)].mCommandQueue->GetNextFence();
}

void SR_QueueManager::Flush()
{
	Action action;
	action.mType = ActionType::Flush;
	action.mQueueType = SR_CommandListType::Copy;
	action.mSemaphore = &SC_Thread::GetCurrentThread()->GetSemaphore();
	PushAction(action);

	action.mSemaphore->Acquire();
}

SC_Ref<SR_CommandList> SR_QueueManager::GetCommandList(const SR_CommandListType& aQueueType, const char* aDebugName)
{
	const uint32 contextIndex = static_cast<uint32>(aQueueType);
	SC_RingArray<SC_Ref<SR_CommandList>>& freeCmdLists = mFreeCommandListsPerContext[contextIndex];

	SC_Ref<SR_CommandList> cmdList;
	{
		SC_MutexLock lock(mFreeCommandListsMutexPerContext[contextIndex]);
		if (!freeCmdLists.IsEmpty())
		{
			cmdList = freeCmdLists.Peek();
			freeCmdLists.Remove();
		}
	}

	if (!cmdList)
		cmdList = SR_RenderDevice::gInstance->CreateCommandList(aQueueType, aDebugName);

	cmdList->Begin();
	return cmdList;
}

void SR_QueueManager::ThreadMain()
{
	while (mIsRunning)
	{
		mHasWorkEvent.Wait();
		mHasWorkEvent.Reset();

		for (;;)
		{
			if (!TryExecAction())
				break;

			TryReturnCommandLists();
		}
	}
}

bool SR_QueueManager::TryExecAction()
{
	for (uint32 contextIndex = 0; contextIndex < static_cast<uint32>(SR_CommandListType::COUNT); ++contextIndex)
	{
		CommandQueueContext& context = mCommandQueueContexts[contextIndex];

		Action currentAction;
		{
			SC_MutexLock lock(context.mQueueMutex);
			if (context.mActionsQueue.IsEmpty())
			{
				lock.Unlock();
				continue;
			}

			currentAction = context.mActionsQueue.Peek();
			context.mActionsQueue.Remove();
		}

		if (ExecAction(currentAction))
			return true;
	}

	return false;
}

bool SR_QueueManager::ExecAction(Action& aAction)
{
	CommandQueueContext& context = mCommandQueueContexts[static_cast<uint32>(aAction.mQueueType)];
	switch (aAction.mType)
	{
	case ActionType::SubmitCommandLists:
	{
		SC_Array<SR_CommandList*> cmdLists(aAction.mCommandLists.Count());
		for (SC_Ref<SR_CommandList>& cmdList : aAction.mCommandLists)
		{
			if (!cmdList->IsClosed())
			{
				SC_ASSERT(false, "CommandList needs to be closed before submitting.");
				return false;
			}

			const SC_Array<SR_Fence>& fenceWaits = cmdList->GetFenceWaits();
			for (const SR_Fence& fence : fenceWaits)
				context.mCommandQueue->InsertWait(fence);

			cmdLists.Add(cmdList.Get());
		}

		context.mCommandQueue->SubmitCommandLists(cmdLists.GetBuffer(), cmdLists.Count());
		context.mCommandQueue->SignalFence(aAction.mFence);

		mCommandListsToReturn.Add(aAction);

		return true;
	}
	case ActionType::SignalFence:
		context.mCommandQueue->SignalFence(aAction.mFence);
		return true;
	case ActionType::InsertWait:
		context.mCommandQueue->InsertWait(aAction.mFence);
		return true;
	case ActionType::Flush:
		aAction.mSemaphore->Release();
		return true;
	case ActionType::Quit:
		SC_Thread::Stop();
		return true;
	}

	return false;
}

void SR_QueueManager::TryReturnCommandLists()
{
	while (!mCommandListsToReturn.IsEmpty())
	{
		Action action = mCommandListsToReturn.Peek();
		if (IsFencePending(action.mFence))
			break;

		mCommandListsToReturn.Remove();

		const uint32 contextIndex = static_cast<uint32>(action.mQueueType);
		SC_MutexLock lock(mFreeCommandListsMutexPerContext[contextIndex]);
		mFreeCommandListsPerContext[contextIndex].Add(action.mCommandLists);
	}
}

void SR_QueueManager::PushAction(const Action& aAction)
{
	CommandQueueContext& context = mCommandQueueContexts[static_cast<uint32>(aAction.mQueueType)];
	{
		SC_MutexLock lock(context.mQueueMutex);
		context.mActionsQueue.Add(aAction);
	}
	mHasWorkEvent.Signal();
}
