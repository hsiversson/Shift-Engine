#pragma once
#include <mutex>
#include <vector> // Only using vector because we don't want to introduce a SC_Array dependency into the Platform lib.
#include "SC_Semaphore.h"
#include "EngineTypes/Functional/SC_Ref.h"

class SC_Event : public SC_ReferenceCounted
{
public:
	SC_Event();
	~SC_Event();

	void Signal();
	bool IsSignalled() const;

	void Reset();
	void Wait();

private:
	enum State
	{
		State_Locked = 0x1,
		State_Signalled = 0x2,
	};

private:
	std::vector<SC_Semaphore*> mWaitingSemaphores;
	volatile uint32 mState;
};