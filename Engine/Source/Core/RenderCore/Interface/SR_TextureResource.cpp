
#include "SR_TextureResource.h"

const SR_TextureResourceProperties& SR_TextureResource::GetProperties() const
{
	return mProperties;
}

void SR_TextureResource::UpdatePixels(const SR_PixelData* aData, uint32 aDataCount)
{
	if (!aData)
		return;

	SC_Ref<SR_CommandList> cmdList = SR_RenderDevice::gInstance->CreateCommandList(SR_CommandListType::Copy);
	cmdList->Begin();
	cmdList->UpdateTexture(this, aData, aDataCount, true);
	cmdList->End();

	SR_Fence fence = SR_RenderDevice::gInstance->GetCommandQueue(SR_CommandListType::Copy)->SubmitCommandList(cmdList.get(), "SR_TextureResource::UpdatePixels");
	SR_RenderDevice::gInstance->WaitForFence(fence);
}

SR_TextureResource::SR_TextureResource(const SR_TextureResourceProperties& aProperties)
	: mProperties(aProperties)
{

}

SR_TextureResource::~SR_TextureResource()
{
}

SR_TextureResourceProperties::SR_TextureResourceProperties(const SR_TextureResourceProperties& aProperties, uint32 aMipOffset)
{
	*this = aProperties;
	assert(aMipOffset < mNumMips);
	mSize.x = SC_Max(mSize.x >> aMipOffset, 1);
	mSize.y = SC_Max(mSize.y >> aMipOffset, 1);
	mSize.z = SC_Max(mSize.z >> aMipOffset, 1);
	mNumMips -= uint16(aMipOffset);
}

SR_TextureResourceProperties::SR_TextureResourceProperties(const SR_TextureResourceProperties& aProperties, const SR_TexturePixelRange& aRange)
{
	*this = aProperties;
	assert(aRange.mLevel.mMipLevel < mNumMips);
	mSize.x = SC_Max(mSize.x >> aRange.mLevel.mMipLevel, 1);
	mSize.y = SC_Max(mSize.y >> aRange.mLevel.mMipLevel, 1);
	mSize.z = SC_Max(mSize.z >> aRange.mLevel.mMipLevel, 1);

	assert(aRange.mLevel.mArrayIndex + aRange.mArraySize <= SC_Max(mArraySize, 1U));
	assert(int(aRange.mDepthIndex + aRange.mDepthSize) <= mSize.z);
	mArraySize = mArraySize ? (uint16)aRange.mArraySize : 0;
	mSize.z = aRange.mDepthSize;
	mNumMips = 1;
}