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

	void EndFrame();

	SR_TempTexture GetTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture = true, bool aIsRenderTarget = false, bool aIsWritable = false);
	
	virtual SR_BufferResource* GetBuffer() = 0;

protected:
	virtual void EndFrameInternal();
	virtual SR_TempTexture GetTextureInternal(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture = true, bool aIsRenderTarget = false, bool aIsWritable = false) = 0;

	SC_Array<SR_TempTexture> mKeepAliveList;
	SC_Queue<SC_Pair<SR_Fence, SC_Array<SR_TempTexture>>> mRemovalQueue;
};

