#pragma once
#include "RenderCore/Interface/SR_TempResourceHeap.h"

#if SR_ENABLE_DX12

class SR_Heap_DX12;
class SR_TempResourceHeap_DX12 : public SR_TempResourceHeap
{
public:
	SR_TempResourceHeap_DX12();
	~SR_TempResourceHeap_DX12();

	bool Init() override;

protected:
	void EndFrameInternal();
	SR_TempTexture GetTextureInternal(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture = true, bool aIsRenderTarget = false, bool aIsWritable = false) override;
	SR_TempBuffer GetBufferInternal(const SR_BufferResourceProperties& aBufferProperties, bool aIsWritable = false) override;
private:
	SC_UniquePtr<SR_Heap_DX12> mResourceHeap_RT_DS_Textures;
	SC_UniquePtr<SR_Heap_DX12> mResourceHeap_RW_Textures;
	SC_UniquePtr<SR_Heap_DX12> mResourceHeap_R_Textures;
};

#endif //SR_ENABLE_DX12

