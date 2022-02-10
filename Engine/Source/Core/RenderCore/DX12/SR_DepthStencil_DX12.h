#pragma once

#if ENABLE_DX12
#include "RenderCore/Interface/SR_DepthStencil.h"

class SR_TextureResource_DX12;

class SR_DepthStencil_DX12 : public SR_DepthStencil
{
public:
	SR_DepthStencil_DX12(const SR_DepthStencilProperties& aProperties, SC_Ref<SR_TextureResource> aResource);
	~SR_DepthStencil_DX12();

	bool Init();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const;

private:
	SR_TextureResource_DX12* mDX12Resource;
};

#endif