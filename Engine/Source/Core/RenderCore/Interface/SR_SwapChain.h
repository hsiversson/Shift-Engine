#pragma once
#include "SR_Fence.h"

class SR_TextureResource;
class SR_RenderTarget;
class SR_Texture;

struct SR_SwapChainProperties
{
	SR_SwapChainProperties()
		: mFormat(SR_Format::UNKNOWN)
		, mFrameLatency(0)
		, mTripleBuffer(false)
		, mFullscreen(false)
	{}

	SC_IntVector2 mSize;
	SR_Format mFormat;

	uint32 mFrameLatency;

	bool mTripleBuffer;
	bool mFullscreen;
};

class SR_SwapChain
{
public:
	SR_SwapChain();
	virtual ~SR_SwapChain();

	const SR_SwapChainProperties& GetProperties() const;

	virtual void Present();
	
	virtual void Update(const SR_SwapChainProperties& aProps);

	SR_Fence& GetLastFrameFence();

	SR_RenderTarget* GetRenderTarget() const;
	SR_Texture* GetTexture() const;

protected:
	struct BackbufferResource
	{
		BackbufferResource() {}
		~BackbufferResource() {}

		SC_Ref<SR_TextureResource> mResource;
		SC_Ref<SR_RenderTarget> mRenderTarget;
		SC_Ref<SR_Texture> mTexture;
	};

protected:
	SR_SwapChainProperties mProperties;
	BackbufferResource mBackbufferResources[3];
	SR_Fence mFrameFence[3];
	BackbufferResource* mCurrentResource;
	uint8 mNumBackbuffers;
	uint8 mCurrentIndex;
};

