#pragma once
#include "RenderCore/Interface/SR_Fence.h"
#include "RenderCore/Interface/SR_CommandList.h"
#include "SR_TaskEvent.h"
#include "Platform/Async/SC_Event.h"

using SR_RenderTaskFunctionSignature = std::function<void()>;


/*
	The RenderTaskManager is responsible for launching tasks related to graphics work.
	Each task will essentially run asynchronously on a separate thread.
	Once the work is finished, it will check if any GPU commands were recorded, and if so, it will schedule the work to the GPU hardware queue.

	Tasks are submitted to the GPU in the same order as they are scheduled

*/

class SR_RenderTaskManager
{
	friend class SR_RenderDevice;
public:
	SR_RenderTaskManager();
	~SR_RenderTaskManager();

	bool Init();

	void SubmitTask(SR_RenderTaskFunctionSignature aTask, const SR_CommandListType& aCommandListType, SR_TaskEvent* aOutEvent);

	// A Split task is a way to ensure a break in a consecutive set of command lists
	// to allow for fences to be inserted more accurately.
	void SubmitSplit(const SR_CommandListType& aCommandListType);

private:
	using InFlightTaskSignature = std::function<SC_Ref<SR_CommandList>()>;

	SC_Ref<SR_CommandList> GetCommandList(const SR_CommandListType& aCommandListType);
	SC_Ref<SR_CommandList> GrabCurrentCommandList();


	class CommandQueueThread : public SC_Thread
	{
	public:
		CommandQueueThread(const SR_CommandListType& aCommandListType, std::queue<SC_Ref<SR_CommandList>>& aFreeCommandLists, SC_Mutex& aFreeCmdListsMutex);
		~CommandQueueThread();

		void ThreadMain() override;

		void SubmitTask(InFlightTaskSignature aTask, SR_TaskEvent* aOutEvent);
		void SubmitSplit();

	private:
		struct InFlightTask
		{
			SC_Future<SC_Ref<SR_CommandList>> mFuture;
			SR_TaskEvent* mEvent;
			bool mIsSplit;
		};

		SC_Mutex mInFlightTasksMutex;
		std::queue<InFlightTask> mInFlightTasks;
		std::queue<SC_Pair<SC_Ref<SR_CommandList>, SR_Fence>> mInFlightCommandLists;
		SC_Event mHasWorkEvent;

		std::queue<SC_Ref<SR_CommandList>>& mFreeCommandLists;
		SC_Mutex& mFreeCmdListsMutex;
		const SR_CommandListType mCommandListType;

		bool mLatestPushedTaskWasSplit;
	};

private:
	SC_UniquePtr<CommandQueueThread> mCommandQueueThreads[static_cast<uint32>(SR_CommandListType::COUNT)];

	std::queue<SC_Ref<SR_CommandList>> mFreeCommandLists[static_cast<uint32>(SR_CommandListType::COUNT)];
	SC_Mutex mFreeCmdListsMutex[static_cast<uint32>(SR_CommandListType::COUNT)];

	static thread_local SR_CommandListType gCurrentTaskType;
	static thread_local SC_Ref<SR_CommandList> gCurrentCommandList;
};

