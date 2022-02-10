#pragma once
#include "RenderCore/Interface/SR_RootSignature.h"

#if ENABLE_DX12

class SR_RootSignature_DX12 : public SR_RootSignature
{
public:
	SR_RootSignature_DX12(const SR_RootSignatureProperties& aProperties);
	~SR_RootSignature_DX12();

	bool Init();

	ID3D12RootSignature* GetD3D12RootSignature() const;

private:
	SR_ComPtr<ID3D12RootSignature> mD3D12RootSignature;
};

#endif

