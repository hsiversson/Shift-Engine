#pragma once
#include "RenderCore/Interface/SR_Fence.h"
#include "RenderCore/Interface/SR_CommandList.h"
#include "SR_TaskEvent.h"
#include "Platform/Async/SC_Event.h"

#if 0
using SR_RenderTaskFunctionSignature = std::function<void()>;

/*
	The QueueManager is responsible for launching tasks related to GPU work.
	Each task will essentially run asynchronously on a separate thread.
	Once the work is finished, it will check if any GPU commands were recorded, and if so, it will schedule the work to the GPU hardware queue.

	Tasks are submitted to the GPU in the same order as they are scheduled
*/

class SR_CommandQueue;
class SR_CommandQueueManager
{
	friend class SR_RenderDevice;
public:
	SR_CommandQueueManager();
	~SR_CommandQueueManager();

	bool Init();

	void SubmitTask(SR_RenderTaskFunctionSignature aTask, const SR_CommandListType& aCommandListType, SR_TaskEvent* aOutEvent);
	SC_Ref<SC_Event> SubmitLightTask(SR_RenderTaskFunctionSignature aTask, const SR_CommandListType& aCommandListType);

	SR_Fence InsertFence(const SR_CommandListType& aCommandListType);
	void InsertWait(const SR_Fence& aFence, const SR_CommandListType& aWaitingQueueType);

	bool IsFencePending(const SR_Fence& aFence);
	bool WaitForFence(const SR_Fence& aFence, bool aBlock = true);

private:
	using RenderTaskSignature = std::function<SC_Ref<SR_CommandList>()>;
	using LightTaskSignature = SR_RenderTaskFunctionSignature;

	SC_Ref<SR_CommandList> GetCommandList(const SR_CommandListType& aCommandListType);
	SC_Ref<SR_CommandList> GrabCurrentCommandList();

	struct CommandQueueWork
	{
		enum class Type
		{
			Task,
			LightTask,
			SignalFence,
			WaitForFence
		};

		RenderTaskSignature mRenderTask;
		LightTaskSignature mLightTask;
		SC_Ref<SC_Event> mLightTaskEvent;
		SR_TaskEvent* mRenderTaskEvent;
		SR_Fence mFence;
		Type mType;
		bool mImmediateExecute;
	};

	class CommandSubmissionThread : public SC_Thread
	{
	public:
		CommandSubmissionThread(const SR_CommandListType& aCommandListType);
		~CommandSubmissionThread();

		void ThreadMain() override;

		void SubmitTask(SC_Future<SC_Ref<SR_CommandList>> aFuture, SR_TaskEvent* aEvent);

		void InsertFence(const SR_Fence& aFence);
		void InsertWait(const SR_Fence& aFence);

		SR_CommandQueue* GetCommandQueue();
		SC_Ref<SR_CommandList> GetCommandList();

	private:
		void SubmitCommandLists(SC_Array<SC_Ref<SR_CommandList>>& aCommandLists, SC_Array<SR_TaskEvent*>& aEventsToSignal);

		enum class WorkType
		{
			Task,
			Signal,
			Wait
		};

		struct TaskWork
		{
			SC_Future<SC_Ref<SR_CommandList>> mFuture;
			SR_TaskEvent* mEvent;
		};

		struct Work
		{
			WorkType mType;
			TaskWork mTask;
			SR_Fence mFence;
		};

		SC_Queue<Work> mWorkQueue;
		SC_Queue<SC_Pair<SC_Ref<SR_CommandList>, SR_Fence>> mInFlightCommandLists;
		SC_Queue<SC_Ref<SR_CommandList>> mFreeCommandLists;
		SC_Mutex mFreeCmdListsMutex;
		SC_Mutex mWorkQueueMutex;
		SC_Event mHasWorkEvent;

		SR_CommandQueue* mCommandQueue;

		const SR_CommandListType mCommandListType;
	};

	class CommandQueueThread : public SC_Thread
	{
	public:
		CommandQueueThread(const SR_CommandListType& aCommandListType, CommandSubmissionThread* aCommandSubmissionThread);
		~CommandQueueThread();

		void ThreadMain() override;

		void SubmitTask(RenderTaskSignature aTask, SR_TaskEvent* aOutEvent);
		void SubmitLightTask(LightTaskSignature aTask, SC_Ref<SC_Event>& aOutEvent);

		SR_Fence InsertFence();
		void InsertWait(const SR_Fence& aFence);

	private:
		void ScheduleSubmit(SC_Future<SC_Ref<SR_CommandList>> aFuture, SR_TaskEvent* aEvent);

		SC_Queue<CommandQueueWork> mWorkQueue;
		SC_Mutex mWorkQueueMutex;
		SC_Event mHasWorkEvent;

		CommandSubmissionThread* mCommandSubmissionThread;

		SR_CommandQueue* mCommandQueue;
		const SR_CommandListType mCommandListType;
	};

private:
	SC_UniquePtr<CommandSubmissionThread> mCommandSubmissionThreads[static_cast<uint32>(SR_CommandListType::COUNT)];
	SC_UniquePtr<CommandQueueThread> mCommandQueueThreads[static_cast<uint32>(SR_CommandListType::COUNT)];

	static thread_local SR_CommandListType gCurrentTaskType;
	static thread_local SC_Ref<SR_CommandList> gCurrentCommandList;
};

#endif