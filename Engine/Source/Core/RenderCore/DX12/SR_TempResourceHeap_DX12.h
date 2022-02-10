#pragma once
#include "RenderCore/Interface/SR_TempResourceHeap.h"

#if ENABLE_DX12

class SR_Heap_DX12;
class SR_TempResourceHeap_DX12 : public SR_TempResourceHeap
{
public:
	SR_TempResourceHeap_DX12();
	~SR_TempResourceHeap_DX12();

	bool Init() override;

	SR_TempTexture GetTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture /* = true */, bool aIsRenderTarget /* = false */, bool aIsWritable /* = false */) override;
	SR_BufferResource* GetBuffer() override;
private:
	SC_UniquePtr<SR_Heap_DX12> mResourceHeap_RT_Textures;
	SC_UniquePtr<SR_Heap_DX12> mResourceHeap_Buffers;
};

#endif //ENABLE_DX12

