
#include "SR_DepthStencil.h"

const SR_DepthStencilProperties& SR_DepthStencil::GetProperties() const
{
	return mProperties;
}

SR_TextureResource* SR_DepthStencil::GetResource() const
{
	return mResource;
}

SR_DepthStencil::SR_DepthStencil(const SR_DepthStencilProperties& aProperties, SC_Ref<SR_TextureResource> aResource)
	: mProperties(aProperties)
	, mResource(aResource)
{

}

SR_DepthStencil::~SR_DepthStencil()
{
	if (mDescriptor.mHeapIndex != SR_Descriptor::gInvalidIndex)
	{
		SR_RenderDevice::gInstance->GetDSVDescriptorHeap()->Free(mDescriptor);
	}
}
