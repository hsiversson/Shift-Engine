#include "SC_Semaphore.h"

SC_Semaphore::SC_Semaphore(uint32 aCount)
	: mCount(aCount)
{

}

SC_Semaphore::~SC_Semaphore()
{

}

void SC_Semaphore::Signal(uint32 aCount)
{
	std::unique_lock lock(mMutex);
	for (uint32 i = 0; i < aCount; ++i)
	{
		++mCount;
		mCondition.notify_one();
	}
}

bool SC_Semaphore::WaitFor(uint32 aMilliseconds)
{
	std::unique_lock lock(mMutex);
	if (!mCondition.wait_for(lock, std::chrono::milliseconds(aMilliseconds), [&]()->bool { return mCount > 0; }))
		return false;

	--mCount;
	return true;
}

void SC_Semaphore::Wait()
{
	std::unique_lock lock(mMutex);
	mCondition.wait(lock, [&]()->bool { return mCount > 0; });
	--mCount;
}
