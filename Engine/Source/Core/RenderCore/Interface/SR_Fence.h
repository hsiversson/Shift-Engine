#pragma once
#include "Platform/Types/SC_TypeDefines.h"
#include "SR_CommandList.h"

class SR_FenceResource
{
public:
	SR_FenceResource() : mType(SR_CommandListType::Unknown) {}
	virtual ~SR_FenceResource() {}

	virtual bool IsPending(uint64 /*aValue*/) { return true; }
	virtual bool Wait(uint64 /*aValue*/, bool /*aBlock*/ = true) {  return true; }

	const SR_CommandListType& GetType() const { return mType; }
protected:
	SR_CommandListType mType;
};

struct SR_Fence
{
	SR_Fence() : mType(SR_CommandListType::Unknown), mValue(0) {}

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

	SR_CommandListType mType;
	uint64 mValue;
	SC_Ref<SR_FenceResource> mResource;
};