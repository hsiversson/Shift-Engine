#pragma once
#include "Platform/Types/SC_TypeDefines.h"
#include "RenderCore/Defines/SR_RenderEnums.h"
#include "RenderCore/Resources/SR_Resource.h"

class SR_FenceResource : public SR_Resource
{
public:
	SR_FenceResource() {}
	virtual ~SR_FenceResource() {}

	virtual bool IsPending(uint64 /*aValue*/) { return false; }
	virtual bool Wait(uint64 /*aValue*/, bool /*aBlock*/ = true) {  return true; }
	virtual uint64 GetNextValue() = 0;
};

struct SR_Fence
{
	SR_Fence();
	SR_Fence(uint64 aValue, const SR_CommandListType& aType);
	bool IsPending() const;
	bool Wait(bool aBlock = true) const;

	bool operator==(const SR_Fence& aOther) const;
	bool operator!=(const SR_Fence& aOther) const;

	uint64 mValue;
	SR_CommandListType mType;
};