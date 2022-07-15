#include "SC_Ref.h"
#include "Platform/Atomics/SC_Atomics.h"

bool SC_RefCountedBase::Increment_NotZero()
{
	uint32 count = mCounter;
	while (count != 0)
	{
		const uint32 oldValue = SC_Atomic::CompareExchange_GetOld(mCounter, count + 1, count);
		if (oldValue == count)
			return true;

		count = oldValue;
	}
	return false;
}

void SC_RefCountedBase::Increment()
{
	SC_Atomic::Increment(mCounter);
}

void SC_RefCountedBase::Decrement()
{
	if (SC_Atomic::Decrement_GetNew(mCounter) == 0)
	{
		Destroy();
		DecrementWeak();
	}
}

void SC_RefCountedBase::IncrementWeak()
{
	SC_Atomic::Increment(mWeakCounter);
}

void SC_RefCountedBase::DecrementWeak()
{
	if (SC_Atomic::Decrement_GetNew(mWeakCounter) == 0)
	{
		DestroySelf();
	}
}

uint32 SC_RefCountedBase::GetCount() const
{
	return mCounter;
}

void* SC_RefCountedBase::GetDeleter(const type_info&) const noexcept
{
	return nullptr;
}
