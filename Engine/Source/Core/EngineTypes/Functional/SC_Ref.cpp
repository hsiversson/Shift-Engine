#include "SC_Ref.h"
#include "Platform/Atomics/SC_Atomics.h"

SC_ReferenceCounted::SC_ReferenceCounted()
	: mReferenceCount(0)
{

}

SC_ReferenceCounted::SC_ReferenceCounted(const SC_ReferenceCounted&)
	: mReferenceCount(0)
{

}

SC_ReferenceCounted& SC_ReferenceCounted::operator=(const SC_ReferenceCounted&)
{
	return *this;
}

void SC_ReferenceCounted::IncrementReference() const
{
	SC_Atomic::Increment_GetNew(mReferenceCount);
}

void SC_ReferenceCounted::DecrementReference() const
{
	for (;;)
	{
		uint32 currentCount = mReferenceCount;
		SC_ASSERT(currentCount > 0, "Trying to decrement Reference Count even though it is already 0");
		if (currentCount == 1)
		{
			if (const_cast<SC_ReferenceCounted*>(this)->CanDestruct(mReferenceCount))
				delete this;
			return;
		}
		
		if (SC_Atomic::CompareExchange(mReferenceCount, currentCount - 1, currentCount))
			return;		
	}
}

uint32 SC_ReferenceCounted::GetReferenceCount() const
{
	return mReferenceCount;
}

SC_ReferenceCounted::~SC_ReferenceCounted()
{
	SC_ASSERT(mReferenceCount == 0, "Reference Count is expected to be 0 but was {}", mReferenceCount);
}

bool SC_ReferenceCounted::CanDestruct(volatile uint32& aReferenceCount)
{
	return SC_Atomic::Decrement_GetNew(aReferenceCount) == 0;
}
