
#include "SR_Texture.h"

const SR_TextureProperties& SR_Texture::GetProperties() const
{
	return mProperties;
}
SR_TextureResource* SR_Texture::GetResource() const
{
	return mResource.get();
}

const SR_TextureResourceProperties& SR_Texture::GetResourceProperties() const
{
	return mResource->GetProperties();
}

const SR_Descriptor& SR_Texture::GetDescriptor() const
{
	return mDescriptor;
}

uint32 SR_Texture::GetDescriptorHeapIndex() const
{
	return mDescriptor.mHeapIndex;
}

bool SR_Texture::IsSRV() const
{
	return !mProperties.mWritable;
}

bool SR_Texture::IsUAV() const
{
	return mProperties.mWritable;
}

SR_Texture::SR_Texture(const SR_TextureProperties& aProperties, const SC_Ref<SR_TextureResource>& aResource)
	: mProperties(aProperties)
	, mResource(aResource)
{

}

SR_Texture::~SR_Texture()
{
	if (mDescriptor.mHeapIndex != SR_Descriptor::gInvalidIndex)
	{
		SR_RenderDevice::gInstance->GetDefaultDescriptorHeap()->Free(mDescriptor);
	}
}
