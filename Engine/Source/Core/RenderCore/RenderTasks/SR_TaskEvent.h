#pragma once
#include "Async/SC_Event.h"
#include "Interface/SR_Fence.h"

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

