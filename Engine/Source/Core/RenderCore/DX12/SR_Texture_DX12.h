#pragma once

#if SR_ENABLE_DX12
#include "RenderCore/Interface/SR_Texture.h"

class SR_TextureResource_DX12;
class SR_Texture_DX12 : public SR_Texture
{
public:
	SR_Texture_DX12(const SR_TextureProperties& aProperties, const SC_Ref<SR_TextureResource>& aTextureResource);
	~SR_Texture_DX12();

	bool Init();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;

private:

	bool InitAsSRV();
	bool InitAsUAV();

	SR_TextureResource_DX12* mDX12Resource;
};

#endif
