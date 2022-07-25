#include "SC_Semaphore.h"

SC_Semaphore::SC_Semaphore(uint32 aCount)
	: mSemaphoreImpl(aCount)
{

}

SC_Semaphore::~SC_Semaphore()
{

}

void SC_Semaphore::Release(uint32 aCount)
{
	SC_ASSERT(aCount > 0);
	mSemaphoreImpl.Release(aCount);
}

bool SC_Semaphore::TimedAcquire(uint32 aMilliseconds)
{
	return mSemaphoreImpl.TimedAcquire(aMilliseconds);
}

void SC_Semaphore::Acquire()
{
	mSemaphoreImpl.Acquire();
}
