#include "SC_Event.h"
#include "SC_Thread.h"
#include "SC_Atomics.h"

SC_Event::SC_Event()
	: mState(0)
{

}

SC_Event::~SC_Event()
{
	while (mState & State_Locked)
	{
		SC_Thread::Yield();
	}

	if (mWaitingSemaphores.size() > 0)
	{
		assert(false);
	}
}

void SC_Event::Signal()
{
	for (;;)
	{
		uint32 prevState = SC_Atomic::CompareExchange_GetOld(mState, (State_Signalled | State_Locked), 0);
		if (prevState & State_Signalled)
			return;
		else if (prevState == 0)
			break;
	}

	for (SC_Semaphore* sem : mWaitingSemaphores)
		sem->Signal();
	mWaitingSemaphores.clear();

	uint32 prevState = SC_Atomic::Exchange_GetOld(mState, State_Signalled);
	assert(prevState == (State_Signalled | State_Locked));
	(void)prevState;
}

bool SC_Event::IsSignalled() const
{
	uint32 currentState = mState;
	return (currentState & State_Signalled) != 0;
}

void SC_Event::Reset()
{
	for (;;)
	{
		uint32 prevState = SC_Atomic::CompareExchange_GetOld(mState, 0, State_Signalled);
		if ((prevState & State_Signalled) == 0 || prevState == State_Signalled)
			return;
	}
}

void SC_Event::Wait()
{
	for (;;)
	{
		uint32 prevState = SC_Atomic::CompareExchange_GetOld(mState, State_Locked, 0);
		if (prevState & State_Signalled)
			return;
		else if (prevState == 0)
			break;
	}

	SC_Thread* thread = SC_Thread::GetCurrentThread();
	SC_Semaphore& semaphore = thread->GetSemaphore();

	mWaitingSemaphores.push_back(&semaphore);

	uint32 prevState = SC_Atomic::Exchange_GetOld(mState, 0);
	assert(prevState == (State_Locked));
	(void)prevState;

	semaphore.Wait();
}
