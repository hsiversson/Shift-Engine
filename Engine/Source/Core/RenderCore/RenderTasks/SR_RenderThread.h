#pragma once
#include "Platform/Async/SC_Thread.h"
#include "RenderCore/Interface/SR_RenderContext.h"

class SR_RenderThread final : public SC_Thread
{
public:
	SR_RenderThread();
	~SR_RenderThread();

	void PostTask(std::function<void()> aTask);

	uint64 GetCurrentFrameIndex() const;
	void Synchronize();

	void EndFrame(uint32 aUpdateThreadFrameIdx);

	static SR_RenderThread* Get();

protected:
	void ThreadMain() override;

	SC_Queue<std::function<void()>> mTaskQueue;
	SC_Mutex mTaskQueueMutex;
	SC_Event mHasWorkEvent;

	SC_Event mEndOfFrameEvent;
	uint64 mCurrentFrameIndex;

	static SR_RenderThread* gInstance;
};

