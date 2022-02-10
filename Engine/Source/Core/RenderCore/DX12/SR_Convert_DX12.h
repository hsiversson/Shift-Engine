#pragma once
#include "SR_Format.h"
#include "SR_RenderEnums.h"

struct SR_TextureResourceProperties;
struct SR_BufferResourceProperties;

#if ENABLE_DX12

SR_Format					SR_D3D12ConvertFormat(DXGI_FORMAT aFormat);
D3D12_RESOURCE_STATES		SR_D3D12ConvertResourceState(uint32 aState);
SR_ResourceState			SR_D3D12ConvertResourceState(D3D12_RESOURCE_STATES aState);
DXGI_FORMAT					SR_D3D12ConvertFormat(SR_Format aFormat);
DXGI_FORMAT					SR_D3D12ConvertToValidSRVFormat(DXGI_FORMAT aFormat);
D3D12_CULL_MODE				SR_D3D12ConvertCullMode(SR_CullMode aCullMode);
D3D12_COMPARISON_FUNC		SR_D3D12ConvertComparisonFunc(SR_ComparisonFunc aComparisonFunc);
D3D12_STENCIL_OP			SR_D3D12ConvertStencilOperator(SR_StencilOperator aStencilOperator);
D3D12_BLEND					SR_D3D12ConvertBlendMode(SR_BlendMode aBlendMode);
D3D12_BLEND_OP				SR_D3D12ConvertBlendFunc(SR_BlendFunc aBlendFunc);
UINT8						SR_D3D12ConvertColorWriteMask(uint8 aWriteMask);
SR_ShaderModel				SR_D3D12ConvertShaderModel(D3D_SHADER_MODEL aShaderModel);
D3D_SHADER_MODEL			SR_D3D12ConvertShaderModel(SR_ShaderModel aShaderModel);
D3D12_FILTER				SR_D3D12ConvertFilter(SR_FilterMode aMinFilter, SR_FilterMode aMagFilter, SR_FilterMode aMipFilter, SR_ComparisonFunc aCompareFunc, uint32 aAnisotropy);
D3D12_TEXTURE_ADDRESS_MODE	SR_D3D12ConvertWrapMode(SR_WrapMode aWrapMode);
D3D12_RESOURCE_DIMENSION	SR_D3D12ConvertDimension(SR_ResourceType aType);
D3D12_RESOURCE_DESC			SR_GetD3D12ResourceDesc(const SR_TextureResourceProperties& aProperties);
D3D12_RESOURCE_DESC			SR_GetD3D12ResourceDesc(const SR_BufferResourceProperties& aProperties);
#endif