#pragma once

#if ENABLE_DX12
#include "Interface/SR_ShaderState.h"

struct ID3D12PipelineState;
struct ID3D12StateObject;

class SR_ShaderState_DX12 : public SR_ShaderState
{
public:
	SR_ShaderState_DX12();
	~SR_ShaderState_DX12();

	bool Init(const SR_ShaderStateProperties& aProperties);

	ID3D12PipelineState* GetD3D12PipelineState() const;
#if ENABLE_RAYTRACING
	const D3D12_DISPATCH_RAYS_DESC& GetDispatchRaysDesc() const;
	ID3D12StateObject* GetD3D12StateObject() const;
#endif
private:

	bool InitDefault(const SR_ShaderStateProperties& aProperties);
#if ENABLE_MESH_SHADERS
	bool InitAsMeshShader(const SR_ShaderStateProperties& aProperties);
#endif
	bool InitAsComputeShader(const SR_ShaderStateProperties& aProperties);
#if ENABLE_RAYTRACING
	bool InitAsRaytracingShader(const SR_ShaderStateProperties& aProperties);
	void CreateRaytracingShaderTable(const SR_ShaderStateProperties& aProperties);
#endif

	SR_ComPtr<ID3D12PipelineState> mD3D12PipelineState;
#if ENABLE_RAYTRACING
	D3D12_DISPATCH_RAYS_DESC mDispatchRaysDesc;
	SR_ComPtr<ID3D12StateObject> mD3D12StateObject;
	SC_Ref<SR_BufferResource> mRaytracingShaderTable;
#endif
};

#endif

