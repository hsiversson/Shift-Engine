#include "SR_Buffer.h"

SR_Buffer::SR_Buffer(const SR_BufferProperties& aProperties, const SC_Ref<SR_BufferResource>& aResource)
	: mProperties(aProperties)
	, mResource(aResource)
{

}

SR_Buffer::~SR_Buffer()
{
	if (mDescriptor.mHeapIndex != SR_Descriptor::gInvalidIndex)
	{
		SR_RenderDevice::gInstance->GetDefaultDescriptorHeap()->Free(mDescriptor);
	}
}

const SR_Descriptor& SR_Buffer::GetDescriptor() const
{
	return mDescriptor;
}

uint32 SR_Buffer::GetDescriptorHeapIndex() const
{
	return mDescriptor.mHeapIndex;
}

const SR_BufferProperties& SR_Buffer::GetProperties() const
{
	return mProperties;
}

SR_BufferResource* SR_Buffer::GetResource() const
{
	return mResource;
}
