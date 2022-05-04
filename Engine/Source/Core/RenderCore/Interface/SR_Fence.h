#pragma once
#include "Platform/Types/SC_TypeDefines.h"
#include "SR_CommandList.h"

struct SR_Fence;
class SR_FenceResource
{
public:
	SR_FenceResource() {}
	virtual ~SR_FenceResource() {}

	virtual bool IsPending(uint64 /*aValue*/) { return true; }
	virtual bool Wait(uint64 /*aValue*/, bool /*aBlock*/ = true) {  return true; }
	virtual SR_Fence GetNextFence() = 0;
};

struct SR_Fence
{
	SR_Fence() : mResource(nullptr), mValue(0), mType(SR_CommandListType::Unknown) {}
	~SR_Fence() {}

	bool IsPending() { return (mResource) ? mResource->IsPending(mValue) : false; }
	bool Wait(bool aBlock = true) { return (mResource) ? mResource->Wait(mValue, aBlock) : true; }

	operator bool()
	{
		return (mType != SR_CommandListType::Unknown) && (mValue != 0);
	}

	bool operator==(const SR_Fence& aOther) const
	{
		if (mType == aOther.mType && mValue == aOther.mValue && mResource == aOther.mResource)
			return true;

		return false;
	}

	bool operator!=(const SR_Fence& aOther) const
	{
		return !(*this == aOther);
	}

	SR_FenceResource* mResource;
	uint64 mValue;
	SR_CommandListType mType;
};