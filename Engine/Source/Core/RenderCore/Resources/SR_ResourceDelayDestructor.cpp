#include "SR_ResourceDelayDestructor.h"

SR_ResourceDelayDestructor::SR_ResourceDelayDestructor()
{

}

SR_ResourceDelayDestructor::~SR_ResourceDelayDestructor()
{

}

bool SR_ResourceDelayDestructor::AddToQueue(SR_Resource* aResource)
{
	if (!gEnabled)
		return false;

	SC_MutexLock lock(mMutex);
	PendingDestruct pending = {};
	pending.mResource = aResource;
	pending.mDestructionFrame = SC_Time::gFrameCounter + 1;
	mQueue.Add(pending);
	return true;
}

void SR_ResourceDelayDestructor::Run()
{
	if (mQueue.IsEmpty())
		return;

	SC_MutexLock lock(mMutex);
	while (!mQueue.IsEmpty())
	{
		PendingDestruct& pending = mQueue.Peek();
		if (pending.mDestructionFrame <= SR_RenderDevice::gLatestFinishedFrame)
		{
			delete pending.mResource;
			mQueue.Remove();
		}
		else
			break;
	}
}
