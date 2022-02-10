#pragma once
#include "SR_Texture.h"
#include "SR_RenderTarget.h"

struct SR_TempTexture
{
	SC_Ref<SR_TextureResource> mResource;
	SC_Ref<SR_Texture> mTexture;
	SC_Ref<SR_Texture> mRWTexture;
	SC_Ref<SR_RenderTarget> mRenderTarget;
};

class SR_TempResourceHeap
{
public:
	SR_TempResourceHeap();
	virtual ~SR_TempResourceHeap();

	virtual bool Init() = 0;

	virtual SR_TempTexture GetTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture = true, bool aIsRenderTarget = false, bool aIsWritable = false) = 0;
	virtual SR_BufferResource* GetBuffer() = 0;

protected:
};

