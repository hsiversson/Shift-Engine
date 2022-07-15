#pragma once
#include "RenderCore/Interface/SR_Fence.h"
#include "RenderCore/Interface/SR_CommandList.h"
#include "SR_TaskEvent.h"
#include "Platform/Async/SC_Event.h"


using SR_RenderTaskFunctionSignature = std::function<void()>;

/*
	The QueueManager is responsible for launching tasks related to GPU work.
	Each task will essentially run asynchronously on a separate thread.
	Once the work is finished, it will check if any GPU commands were recorded, and if so, it will schedule the work to the GPU hardware queue.

	Tasks are submitted to the GPU in the same order as they are scheduled
*/

class SR_CommandQueue;
class SR_QueueManager
{
	friend class SR_RenderDevice;
public:
	SR_QueueManager();
	~SR_QueueManager();

	bool Init();

	void SubmitTask(SR_RenderTaskFunctionSignature aTask, const SR_CommandListType& aCommandListType, SR_TaskEvent* aOutEvent);

	SR_Fence InsertFence(const SR_CommandListType& aCommandListType);
	void InsertFence(const SR_CommandListType& aCommandListType, SR_Fence& aFence);
	void InsertWait(const SR_Fence& aFence, const SR_CommandListType& aCommandListType);

private:
	using InFlightTaskSignature = std::function<SC_Ref<SR_CommandList>()>;

	SC_Ref<SR_CommandList> GetCommandList(const SR_CommandListType& aCommandListType);
	SC_Ref<SR_CommandList> GrabCurrentCommandList();

	struct CommandQueueWork
	{
		enum class Type
		{
			Task,
			SignalFence,
			WaitForFence
		};

		SC_Future<SC_Ref<SR_CommandList>> mFuture;
		SR_TaskEvent* mEvent;
		SR_Fence mFence;
		Type mType;
	};

	class CommandQueueThread : public SC_Thread
	{
	public:
		CommandQueueThread(const SR_CommandListType& aCommandListType);
		~CommandQueueThread();

		void ThreadMain() override;

		void SubmitTask(InFlightTaskSignature aTask, SR_TaskEvent* aOutEvent);

		SR_Fence InsertFence();
		void InsertFence(const SR_Fence& aFence);
		void InsertWait(const SR_Fence& aFence);

		SC_Ref<SR_CommandList> GetCommandList();

	private:
		void SubmitCommandLists(SC_Array<SR_CommandList*>& aCommandLists, SC_Array<CommandQueueWork>& aWorkToSignal);

		SC_Queue<CommandQueueWork> mWorkQueue;
		SC_Mutex mWorkQueueMutex;
		SC_Event mHasWorkEvent;

		SR_CommandQueue* mCommandQueue;
		SC_Ref<SR_FenceResource> mCommandQueueFence;

		SC_Queue<SC_Pair<SC_Ref<SR_CommandList>, SR_Fence>> mInFlightCommandLists;

		SC_Queue<SC_Ref<SR_CommandList>> mFreeCommandLists;
		SC_Mutex mFreeCmdListsMutex;

		const SR_CommandListType mCommandListType;
	};

private:
	SC_UniquePtr<CommandQueueThread> mCommandQueueThreads[static_cast<uint32>(SR_CommandListType::COUNT)];

	static thread_local SR_CommandListType gCurrentTaskType;
	static thread_local SC_Ref<SR_CommandList> gCurrentCommandList;
};

