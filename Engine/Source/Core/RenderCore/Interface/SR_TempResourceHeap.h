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

struct SR_TempBuffer
{
	SC_Ref<SR_BufferResource> mResource;
	SC_Ref<SR_Buffer> mBuffer;
	SC_Ref<SR_Buffer> mRWBuffer;
};

class SR_TempResourceHeap
{
public:
	SR_TempResourceHeap();
	virtual ~SR_TempResourceHeap();

	virtual bool Init() = 0;

	void EndFrame();

	SR_TempTexture GetTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture = true, bool aIsRenderTarget = false, bool aIsWritable = false);
	SR_TempBuffer GetBuffer(const SR_BufferResourceProperties& aBufferProperties, bool aIsWritable = false);

protected:
	virtual void EndFrameInternal();
	virtual SR_TempTexture GetTextureInternal(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture = true, bool aIsRenderTarget = false, bool aIsWritable = false) = 0;
	virtual SR_TempBuffer GetBufferInternal(const SR_BufferResourceProperties& aBufferProperties, bool aIsWritable = false) = 0;

	SC_Array<SR_TempTexture> mTextureKeepAliveList;
	SC_Array<SR_TempBuffer> mBufferKeepAliveList;
	SC_Queue<SC_Pair<SR_Fence, SC_Array<SR_TempTexture>>> mTempTextureRemovalQueue;
	SC_Queue<SC_Pair<SR_Fence, SC_Array<SR_TempBuffer>>> mTempBufferRemovalQueue;
};

