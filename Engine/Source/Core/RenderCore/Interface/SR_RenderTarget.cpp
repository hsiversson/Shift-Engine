
#include "SR_RenderTarget.h"

const SR_RenderTargetProperties& SR_RenderTarget::GetProperties() const
{
	return mProperties;
}

SR_TextureResource* SR_RenderTarget::GetResource() const
{
	return mResource;
}

SR_RenderTarget::SR_RenderTarget(const SR_RenderTargetProperties& aProperties, SC_Ref<SR_TextureResource> aResource)
	: mProperties(aProperties)
	, mResource(aResource)
{

}

SR_RenderTarget::~SR_RenderTarget()
{
	if (mDescriptor.mHeapIndex != SR_Descriptor::gInvalidIndex)
	{
		SR_RenderDevice::gInstance->GetRTVDescriptorHeap()->Free(mDescriptor);
	}
}
