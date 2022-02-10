#pragma once
#include "Platform/Async/SC_Event.h"
#include "RenderCore/Interface/SR_Fence.h"

struct SR_TaskEvent
{
	void Reset()
	{
		mCPUEvent.Reset();
		mFence = SR_Fence();
	}

	SC_Event mCPUEvent;
	SR_Fence mFence;
};

