#pragma once
#include "RenderCore/Interface/SR_Fence.h"
#include "Platform/Async/SC_Thread.h"

class SR_CommandQueue;
class SR_CommandList;
class SR_QueueManager final : public SC_Thread
{
public:
	SR_QueueManager();
	~SR_QueueManager();

	bool Init();

	void SubmitCommandLists(const SR_CommandListType& aQueueType, const SR_Fence& aFence, const SC_Array<SC_Ref<SR_CommandList>>& aCommandLists);

	void SignalFence(const SR_CommandListType& aQueueType, const SR_Fence& aFence);
	void InsertFence(const SR_CommandListType& aQueueType, const SR_Fence& aFence);

	bool IsFencePending(const SR_Fence& aFence) const;
	bool WaitForFence(const SR_Fence& aFence, bool aBlock = true) const;

	SR_Fence GetNextFence(const SR_CommandListType& aQueueType) const;

	void Flush();

	SC_Ref<SR_CommandList> GetCommandList(const SR_CommandListType& aQueueType);

protected:

	enum class ActionType
	{
		SubmitCommandLists,
		SignalFence,
		InsertWait,
		Flush,
		Quit
	};

	struct Action
	{
		SC_Array<SC_Ref<SR_CommandList>> mCommandLists;
		ActionType mType;
		SR_Fence mFence;
		SC_Semaphore* mSemaphore;
		SR_CommandListType mQueueType;
	};

	struct CommandQueueContext
	{
		SC_Mutex mQueueMutex;
		SC_RingArray<Action> mActionsQueue;
		SR_CommandQueue* mCommandQueue;
	};

	CommandQueueContext mCommandQueueContexts[static_cast<uint32>(SR_CommandListType::COUNT)];
	SC_RingArray<SC_Ref<SR_CommandList>> mFreeCommandListsPerContext[static_cast<uint32>(SR_CommandListType::COUNT)];
	SC_Mutex mFreeCommandListsMutexPerContext[static_cast<uint32>(SR_CommandListType::COUNT)];
	SC_RingArray<Action> mCommandListsToReturn;
	SC_Event mHasWorkEvent;

protected:
	void ThreadMain() override;
	bool TryExecAction();
	bool ExecAction(Action& aAction);
	void TryReturnCommandLists();
	void PushAction(const Action& aAction);
};

