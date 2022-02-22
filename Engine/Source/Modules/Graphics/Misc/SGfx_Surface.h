#pragma once
#include "RenderCore/Interface/SR_Texture.h"
#include "RenderCore/Interface/SR_RenderTarget.h"

struct SGfx_Surface
{
	bool Init(const SC_IntVector& aSize, const SR_Format& aFormat, bool aAllowRenderTarget = false, bool aAllowUnorderedAccess = false, const char* aDebugName = nullptr);

	const SR_TextureResourceProperties& GetResourceProperties() const { return mResource->GetProperties(); }

	SC_Ref<SR_TextureResource> mResource;
	SC_Ref<SR_Texture> mTexture;
	SC_Ref<SR_Texture> mTextureRW;
	SC_Ref<SR_RenderTarget> mRenderTarget;
};