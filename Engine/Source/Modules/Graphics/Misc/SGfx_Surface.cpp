#include "SGfx_Surface.h"
#include "RenderCore/Interface/SR_RenderDevice.h"

bool SGfx_Surface::Init(const SC_IntVector& aSize, const SR_Format& aFormat, bool aAllowRenderTarget, bool aAllowUnorderedAccess, const char* aDebugName)
{
	SR_TextureResourceProperties resourceProps;
	resourceProps.mAllowRenderTarget = aAllowRenderTarget;
	resourceProps.mAllowUnorderedAccess = aAllowUnorderedAccess;
	resourceProps.mFormat = aFormat;
	resourceProps.mSize = aSize;
	resourceProps.mArraySize = 1;
	resourceProps.mNumMips = 1;
	resourceProps.mType = SR_ResourceType::Texture2D;
	resourceProps.mDebugName = aDebugName;

	mResource = SR_RenderDevice::gInstance->CreateTextureResource(resourceProps);

	SR_TextureProperties texProps(aFormat);
	mTexture = SR_RenderDevice::gInstance->CreateTexture(texProps, mResource);

	if (aAllowUnorderedAccess)
	{
		texProps.mWritable = true;
		mTextureRW = SR_RenderDevice::gInstance->CreateTexture(texProps, mResource);
	}

	if (aAllowRenderTarget)
	{
		SR_RenderTargetProperties rtProps(aFormat);
		mRenderTarget = SR_RenderDevice::gInstance->CreateRenderTarget(rtProps, mResource);
	}

	return true;
}