#pragma once
#include "Platform/Async/SC_Thread.h"
#include "RenderCore/Interface/SR_RenderContext.h"

using SR_RenderTaskSignature = std::function<void()>;

class SR_RenderThread final : public SC_Thread
{
public:
	SR_RenderThread(const SR_CommandListType& aContextType);
	~SR_RenderThread();

	SC_Ref<SR_TaskEvent> PostTask(SR_RenderTaskSignature aTask);

	static thread_local SR_CommandListType gCurrentContextType;
	static thread_local SC_Ref<SR_CommandList> gCurrentCommandList;
protected:
	void ThreadMain() override;

	using InternalTaskSignature = std::function<SC_Ref<SR_CommandList>()>;
	struct Task
	{
		InternalTaskSignature mTask;
		SC_Ref<SR_TaskEvent> mTaskEvent;
	};

	SC_RingArray<Task> mTaskQueue;
	SC_Mutex mTaskQueueMutex;
	SC_Event mHasWorkEvent;

	const SR_CommandListType mContextType;
};