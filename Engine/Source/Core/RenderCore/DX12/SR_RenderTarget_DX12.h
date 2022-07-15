#pragma once

#if SR_ENABLE_DX12
#include "RenderCore/Interface/SR_RenderTarget.h"

class SR_TextureResource_DX12;

class SR_RenderTarget_DX12 : public SR_RenderTarget
{
public:
	SR_RenderTarget_DX12(const SR_RenderTargetProperties& aProperties, SC_Ref<SR_TextureResource> aResource);
	~SR_RenderTarget_DX12();

	bool Init();
	
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const;

private:
};

#endif