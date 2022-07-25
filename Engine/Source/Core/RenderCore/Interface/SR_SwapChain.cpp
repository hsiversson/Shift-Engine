#include "SR_SwapChain.h"

SR_SwapChain::SR_SwapChain()
	: mCurrentResource(nullptr)
	, mCurrentIndex(0)
{

}

SR_SwapChain::~SR_SwapChain()
{

}

const SR_SwapChainProperties& SR_SwapChain::GetProperties() const
{
	return mProperties;
}

void SR_SwapChain::Present()
{
}

void SR_SwapChain::Update(const SR_SwapChainProperties& /*aProps*/)
{
}

SR_RenderTarget* SR_SwapChain::GetRenderTarget() const
{
	return mCurrentResource->mRenderTarget;
}

SR_Texture* SR_SwapChain::GetTexture() const
{
	return mCurrentResource->mTexture;
}
