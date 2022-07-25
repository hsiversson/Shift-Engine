#include "SR_Fence.h"
#include "SR_RenderDevice.h"

SR_Fence::SR_Fence() 
    : mValue(0)
    , mType(SR_CommandListType::Unknown) 
{
}

SR_Fence::SR_Fence(uint64 aValue, const SR_CommandListType& aType) 
    : mValue(aValue)
    , mType(aType)
{
}

bool SR_Fence::IsPending() const
{
    if (mType == SR_CommandListType::Unknown)
        return false;

    return SR_RenderDevice::gInstance->GetCommandQueue(mType)->IsFencePending(*this);
}

bool SR_Fence::Wait(bool aBlock) const
{
    return SR_RenderDevice::gInstance->WaitForFence(*this, aBlock);
}

bool SR_Fence::operator==(const SR_Fence& aOther) const
{
    return (mValue == aOther.mValue) && (mType == aOther.mType);
}

bool SR_Fence::operator!=(const SR_Fence& aOther) const
{
    return !(*this == aOther);
}
