#include "SR_Convert_DX12.h"
#include "Interface/SR_TextureResource.h"
#include "Interface/SR_BufferResource.h"

#if ENABLE_DX12

SR_Format SR_D3D12ConvertFormat(DXGI_FORMAT aFormat)
{
	switch (aFormat)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		return SR_Format::RGBA32_TYPELESS;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return SR_Format::RGBA32_FLOAT;
	case DXGI_FORMAT_R32G32B32A32_UINT:
		return SR_Format::RGBA32_UINT;
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return SR_Format::RGBA32_SINT;
	case DXGI_FORMAT_R32G32B32_TYPELESS:
		return SR_Format::RGB32_TYPELESS;
	case DXGI_FORMAT_R32G32B32_FLOAT:
		return SR_Format::RGB32_FLOAT;
	case DXGI_FORMAT_R32G32B32_UINT:
		return SR_Format::RGB32_UINT;
	case DXGI_FORMAT_R32G32B32_SINT:
		return SR_Format::RGB32_SINT;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		return SR_Format::RGBA16_TYPELESS;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return SR_Format::RGBA16_FLOAT;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return SR_Format::RGBA16_UNORM;
	case DXGI_FORMAT_R16G16B16A16_UINT:
		return SR_Format::RGBA16_UINT;
	case DXGI_FORMAT_R16G16B16A16_SNORM:
		return SR_Format::RGBA16_SNORM;
	case DXGI_FORMAT_R16G16B16A16_SINT:
		return SR_Format::RGBA16_SINT;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		return SR_Format::RGBA8_TYPELESS;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return SR_Format::RGBA8_UNORM;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return SR_Format::RGBA8_UNORM_SRGB;
	case DXGI_FORMAT_R8G8B8A8_UINT:
		return SR_Format::RGBA8_UINT;
	case DXGI_FORMAT_R8G8B8A8_SNORM:
		return SR_Format::RGBA8_SNORM;
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return SR_Format::RGBA8_SINT;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return SR_Format::BGRA8_UNORM;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		return SR_Format::RGB10A2_TYPELESS;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
		return SR_Format::RGB10A2_UNORM;
	case DXGI_FORMAT_R10G10B10A2_UINT:
		return SR_Format::RGB10A2_UINT;
	case DXGI_FORMAT_R11G11B10_FLOAT:
		return SR_Format::RG11B10_FLOAT;
	case DXGI_FORMAT_R32G32_TYPELESS:
		return SR_Format::RG32_TYPELESS;
	case DXGI_FORMAT_R32G32_FLOAT:
		return SR_Format::RG32_FLOAT;
	case DXGI_FORMAT_R32G32_UINT:
		return SR_Format::RG32_UINT;
	case DXGI_FORMAT_R32G32_SINT:
		return SR_Format::RG32_SINT;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		return SR_Format::D32_FLOAT_S8X24_UINT;
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		return SR_Format::R32_FLOAT_X8X24_TYPELESS;
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return SR_Format::X32_TYPELESS_G8X24_UINT;
	case DXGI_FORMAT_R16G16_TYPELESS:
		return SR_Format::RG16_TYPELESS;
	case DXGI_FORMAT_R16G16_FLOAT:
		return SR_Format::RG16_FLOAT;
	case DXGI_FORMAT_R16G16_UNORM:
		return SR_Format::RG16_UNORM;
	case DXGI_FORMAT_R16G16_UINT:
		return SR_Format::RG16_UINT;
	case DXGI_FORMAT_R16G16_SNORM:
		return SR_Format::RG16_SNORM;
	case DXGI_FORMAT_R16G16_SINT:
		return SR_Format::RG16_SINT;
	case DXGI_FORMAT_R8G8_TYPELESS:
		return SR_Format::RG8_TYPELESS;
	case DXGI_FORMAT_R8G8_UNORM:
		return SR_Format::RG8_UNORM;
	case DXGI_FORMAT_R8G8_UINT:
		return SR_Format::RG8_UINT;
	case DXGI_FORMAT_R8G8_SNORM:
		return SR_Format::RG8_SNORM;
	case DXGI_FORMAT_R8G8_SINT:
		return SR_Format::RG8_SINT;
	case DXGI_FORMAT_R32_TYPELESS:
		return SR_Format::R32_TYPELESS;
	case DXGI_FORMAT_R32_FLOAT:
		return SR_Format::R32_FLOAT;
	case DXGI_FORMAT_D32_FLOAT:
		return SR_Format::D32_FLOAT;
	case DXGI_FORMAT_R32_UINT:
		return SR_Format::R32_UINT;
	case DXGI_FORMAT_R32_SINT:
		return SR_Format::R32_SINT;
	case DXGI_FORMAT_R16_TYPELESS:
		return SR_Format::R16_TYPELESS;
	case DXGI_FORMAT_R16_FLOAT:
		return SR_Format::R16_FLOAT;
	case DXGI_FORMAT_R16_UNORM:
		return SR_Format::R16_UNORM;
	case DXGI_FORMAT_D16_UNORM:
		return SR_Format::D16_UNORM;
	case DXGI_FORMAT_R16_UINT:
		return SR_Format::R16_UINT;
	case DXGI_FORMAT_R16_SNORM :
		return SR_Format::R16_SNORM;
	case DXGI_FORMAT_R16_SINT:
		return SR_Format::R16_SINT;
	case DXGI_FORMAT_R8_TYPELESS:
		return SR_Format::R8_TYPELESS;
	case DXGI_FORMAT_R8_UNORM:
		return SR_Format::R8_UNORM;
	case DXGI_FORMAT_R8_UINT:
		return SR_Format::R8_UINT;
	case DXGI_FORMAT_R8_SNORM:
		return SR_Format::R8_SNORM;
	case DXGI_FORMAT_BC1_UNORM:
		return SR_Format::BC1_UNORM;
	case DXGI_FORMAT_BC2_UNORM:
		return SR_Format::BC2_UNORM;
	case DXGI_FORMAT_BC3_UNORM:
		return SR_Format::BC3_UNORM;
	case DXGI_FORMAT_BC4_UNORM:
		return SR_Format::BC4_UNORM;
	case DXGI_FORMAT_BC4_SNORM:
		return SR_Format::BC4_SNORM;
	case DXGI_FORMAT_BC5_UNORM:
		return SR_Format::BC5_UNORM;
	case DXGI_FORMAT_BC5_SNORM:
		return SR_Format::BC5_SNORM;
	case DXGI_FORMAT_BC7_UNORM:
		return SR_Format::BC7;
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return SR_Format::BC7_SRGB;

	case DXGI_FORMAT_UNKNOWN:
	default:
		return SR_Format::UNKNOWN;
	}
}

D3D12_RESOURCE_STATES SR_D3D12ConvertResourceState(uint32 aState)
{
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
	switch (aState)
	{
	case SR_ResourceState_Read:
		state |= (D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		break;
	case SR_ResourceState_UnorderedAccess:
		state |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		break;
	case SR_ResourceState_RenderTarget:
		state |= D3D12_RESOURCE_STATE_RENDER_TARGET;
		break;
	case SR_ResourceState_DepthRead:
		state |= D3D12_RESOURCE_STATE_DEPTH_READ;
		break;
	case SR_ResourceState_DepthWrite:
		state |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
		break;
	case SR_ResourceState_AccelerationStructure:
		state |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		break;
	case SR_ResourceState_CopyDst:
		state |= D3D12_RESOURCE_STATE_COPY_DEST;
		break;
	case SR_ResourceState_CopySrc:
		state |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		break;
	case SR_ResourceState_Indirect:
		state |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		break;
	case SR_ResourceState_Common:
	default:
		state |= D3D12_RESOURCE_STATE_COMMON;
		break;
	}

	return state;
}

SR_ResourceState SR_D3D12ConvertResourceState(D3D12_RESOURCE_STATES /*aState*/)
{
	return SR_ResourceState();
}

DXGI_FORMAT SR_D3D12ConvertFormat(SR_Format aFormat)
{
	switch (aFormat)
	{
	case SR_Format::RGBA32_TYPELESS:
		return DXGI_FORMAT_R32G32B32A32_TYPELESS;
	case SR_Format::RGBA32_FLOAT:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case SR_Format::RGBA32_UINT:
		return DXGI_FORMAT_R32G32B32A32_UINT;
	case SR_Format::RGBA32_SINT:
		return DXGI_FORMAT_R32G32B32A32_SINT;
	case SR_Format::RGB32_TYPELESS:
		return DXGI_FORMAT_R32G32B32_TYPELESS;
	case SR_Format::RGB32_FLOAT:
		return DXGI_FORMAT_R32G32B32_FLOAT;
	case SR_Format::RGB32_UINT:
		return DXGI_FORMAT_R32G32B32_UINT;
	case SR_Format::RGB32_SINT:
		return DXGI_FORMAT_R32G32B32_SINT;
	case SR_Format::RGBA16_TYPELESS:
		return DXGI_FORMAT_R16G16B16A16_TYPELESS;
	case SR_Format::RGBA16_FLOAT:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case SR_Format::RGBA16_UNORM:
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	case SR_Format::RGBA16_UINT:
		return DXGI_FORMAT_R16G16B16A16_UINT;
	case SR_Format::RGBA16_SNORM:
		return DXGI_FORMAT_R16G16B16A16_SNORM;
	case SR_Format::RGBA16_SINT:
		return DXGI_FORMAT_R16G16B16A16_SINT;
	case SR_Format::RGBA8_TYPELESS:
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;
	case SR_Format::RGBA8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case SR_Format::RGBA8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case SR_Format::RGBA8_UINT:
		return DXGI_FORMAT_R8G8B8A8_UINT;
	case SR_Format::RGBA8_SNORM:
		return DXGI_FORMAT_R8G8B8A8_SNORM;
	case SR_Format::RGBA8_SINT:
		return DXGI_FORMAT_R8G8B8A8_SINT;
	case SR_Format::BGRA8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case SR_Format::RGB10A2_TYPELESS:
		return DXGI_FORMAT_R10G10B10A2_TYPELESS;
	case SR_Format::RGB10A2_UNORM:
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	case SR_Format::RGB10A2_UINT:
		return DXGI_FORMAT_R10G10B10A2_UINT;
	case SR_Format::RG11B10_FLOAT:
		return DXGI_FORMAT_R11G11B10_FLOAT;
	case SR_Format::RG32_TYPELESS:
		return DXGI_FORMAT_R32G32_TYPELESS;
	case SR_Format::RG32_FLOAT:
		return DXGI_FORMAT_R32G32_FLOAT;
	case SR_Format::RG32_UINT:
		return DXGI_FORMAT_R32G32_UINT;
	case SR_Format::RG32_SINT:
		return DXGI_FORMAT_R32G32_SINT;
	case SR_Format::D32_FLOAT_S8X24_UINT:
		return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	case SR_Format::R32_FLOAT_X8X24_TYPELESS:
		return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	case SR_Format::X32_TYPELESS_G8X24_UINT:
		return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
	case SR_Format::RG16_TYPELESS:
		return DXGI_FORMAT_R16G16_TYPELESS;
	case SR_Format::RG16_FLOAT:
		return DXGI_FORMAT_R16G16_FLOAT;
	case SR_Format::RG16_UNORM:
		return DXGI_FORMAT_R16G16_UNORM;
	case SR_Format::RG16_UINT:
		return DXGI_FORMAT_R16G16_UINT;
	case SR_Format::RG16_SNORM:
		return DXGI_FORMAT_R16G16_SNORM;
	case SR_Format::RG16_SINT:
		return DXGI_FORMAT_R16G16_SINT;
	case SR_Format::RG8_TYPELESS:
		return DXGI_FORMAT_R8G8_TYPELESS;
	case SR_Format::RG8_UNORM:
		return DXGI_FORMAT_R8G8_UNORM;
	case SR_Format::RG8_UINT:
		return DXGI_FORMAT_R8G8_UINT;
	case SR_Format::RG8_SNORM:
		return DXGI_FORMAT_R8G8_SNORM;
	case SR_Format::RG8_SINT:
		return DXGI_FORMAT_R8G8_SINT;
	case SR_Format::R32_TYPELESS:
		return DXGI_FORMAT_R32_TYPELESS;
	case SR_Format::R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
	case SR_Format::D32_FLOAT:
		return DXGI_FORMAT_D32_FLOAT;
	case SR_Format::R32_UINT:
		return DXGI_FORMAT_R32_UINT;
	case SR_Format::R32_SINT:
		return DXGI_FORMAT_R32_SINT;
	case SR_Format::R16_TYPELESS:
		return DXGI_FORMAT_R16_TYPELESS;
	case SR_Format::R16_FLOAT:
		return DXGI_FORMAT_R16_FLOAT;
	case SR_Format::R16_UNORM:
		return DXGI_FORMAT_R16G16_UNORM;
	case SR_Format::D16_UNORM:
		return DXGI_FORMAT_D16_UNORM;
	case SR_Format::R16_UINT:
		return DXGI_FORMAT_R16_UINT;
	case SR_Format::R16_SNORM:
		return DXGI_FORMAT_R16_SNORM;
	case SR_Format::R16_SINT:
		return DXGI_FORMAT_R16_SINT;
	case SR_Format::R8_TYPELESS:
		return DXGI_FORMAT_R8_TYPELESS;
	case SR_Format::R8_UNORM:
		return DXGI_FORMAT_R8_UNORM;
	case SR_Format::R8_UINT:
		return DXGI_FORMAT_R8_UINT;
	case SR_Format::R8_SNORM:
		return DXGI_FORMAT_R8_SNORM;
	case SR_Format::R8_SINT:
		return DXGI_FORMAT_R8_SINT;
	case SR_Format::BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM;
	case SR_Format::BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM;
	case SR_Format::BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM;
	case SR_Format::BC4_UNORM:
		return DXGI_FORMAT_BC4_UNORM;
	case SR_Format::BC4_SNORM:
		return DXGI_FORMAT_BC4_SNORM;
	case SR_Format::BC5_UNORM:
		return DXGI_FORMAT_BC5_UNORM;
	case SR_Format::BC5_SNORM:
		return DXGI_FORMAT_BC5_SNORM;
	case SR_Format::BC7:
		return DXGI_FORMAT_BC7_UNORM;
	case SR_Format::BC7_SRGB:
		return DXGI_FORMAT_BC7_UNORM_SRGB;
	case SR_Format::UNKNOWN:
	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}

DXGI_FORMAT SR_D3D12ConvertToValidSRVFormat(DXGI_FORMAT aFormat)
{
	switch (aFormat)
	{
	case DXGI_FORMAT_D32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
	case DXGI_FORMAT_D16_UNORM:
		return DXGI_FORMAT_R16_UNORM;
	}
	return aFormat;
}

D3D12_CULL_MODE SR_D3D12ConvertCullMode(SR_CullMode aCullMode)
{
	switch (aCullMode)
	{
	case SR_CullMode::None:
		return D3D12_CULL_MODE_NONE;
	case SR_CullMode::Front:
		return D3D12_CULL_MODE_FRONT;
	case SR_CullMode::Back:
		return D3D12_CULL_MODE_BACK;
	default:
		break;
	}
	assert(false && "Unknown cull mode DX12");
	return D3D12_CULL_MODE_NONE;
}

D3D12_COMPARISON_FUNC SR_D3D12ConvertComparisonFunc(SR_ComparisonFunc aComparisonFunc)
{
	switch (aComparisonFunc)
	{
	case SR_ComparisonFunc::Never:
		return D3D12_COMPARISON_FUNC_NEVER;
	case SR_ComparisonFunc::Less:
		return D3D12_COMPARISON_FUNC_LESS;
	case SR_ComparisonFunc::Equal:
		return D3D12_COMPARISON_FUNC_EQUAL;
	case SR_ComparisonFunc::LessEqual:
		return D3D12_COMPARISON_FUNC_LESS_EQUAL;
	case SR_ComparisonFunc::Greater:
		return D3D12_COMPARISON_FUNC_GREATER;
	case SR_ComparisonFunc::NotEqual:
		return D3D12_COMPARISON_FUNC_NOT_EQUAL;
	case SR_ComparisonFunc::GreaterEqual:
		return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	case SR_ComparisonFunc::Always:
		return D3D12_COMPARISON_FUNC_ALWAYS;
	default:
		break;
	}
	assert(false && "Unknown comparison function DX12");
	return D3D12_COMPARISON_FUNC_NEVER;
}

D3D12_STENCIL_OP SR_D3D12ConvertStencilOperator(SR_StencilOperator aStencilOperator)
{
	switch (aStencilOperator)
	{
	case SR_StencilOperator::Keep:
		return D3D12_STENCIL_OP_KEEP;
	case SR_StencilOperator::Zero:
		return D3D12_STENCIL_OP_ZERO;
	case SR_StencilOperator::Replace:
		return D3D12_STENCIL_OP_REPLACE;
	case SR_StencilOperator::IncrementSaturate:
		return D3D12_STENCIL_OP_INCR_SAT;
	case SR_StencilOperator::DecrementSaturate:
		return D3D12_STENCIL_OP_DECR_SAT;
	case SR_StencilOperator::Invert:
		return D3D12_STENCIL_OP_INVERT;
	case SR_StencilOperator::Increment:
		return D3D12_STENCIL_OP_INCR;
	case SR_StencilOperator::Decrement:
		return D3D12_STENCIL_OP_DECR;
	default:
		break;
	}
	assert(false && "Unknown stencil operator DX12");
	return D3D12_STENCIL_OP_KEEP;
}

D3D12_BLEND SR_D3D12ConvertBlendMode(SR_BlendMode aBlendMode)
{
	switch (aBlendMode)
	{
	case SR_BlendMode::Zero:
		return D3D12_BLEND_ZERO;
	case SR_BlendMode::One:
		return D3D12_BLEND_ONE;
	case SR_BlendMode::SrcColor:
		return D3D12_BLEND_SRC_COLOR;
	case SR_BlendMode::OneMinusSrcColor:
		return D3D12_BLEND_INV_SRC_COLOR;
	case SR_BlendMode::SrcAlpha:
		return D3D12_BLEND_SRC_ALPHA;
	case SR_BlendMode::OneMinusSrcAlpha:
		return D3D12_BLEND_INV_SRC_ALPHA;
	case SR_BlendMode::DstAlpha:
		return D3D12_BLEND_DEST_ALPHA;
	case SR_BlendMode::OneMinusDstAlpha:
		return D3D12_BLEND_INV_DEST_ALPHA;
	case SR_BlendMode::DstColor:
		return D3D12_BLEND_DEST_COLOR;
	case SR_BlendMode::OneMinusDstColor:
		return D3D12_BLEND_INV_DEST_COLOR;
	case SR_BlendMode::SrcAlphaSaturate:
		return D3D12_BLEND_SRC_ALPHA_SAT;
	case SR_BlendMode::BlendFactor:
		return D3D12_BLEND_BLEND_FACTOR;
	case SR_BlendMode::OneMinusBlendFactor:
		return D3D12_BLEND_INV_BLEND_FACTOR;
	case SR_BlendMode::Src1Color:
		return D3D12_BLEND_SRC1_COLOR;
	case SR_BlendMode::OneMinusSrc1Color:
		return D3D12_BLEND_INV_SRC1_COLOR;
	case SR_BlendMode::Src1Alpha:
		return D3D12_BLEND_SRC1_ALPHA;
	case SR_BlendMode::OneMinusSrc1Alpha:
		return D3D12_BLEND_INV_SRC1_ALPHA;
	default:
		break;
	}
	assert(false && "Unknown blend mode DX12");
	return D3D12_BLEND_ZERO;
}

D3D12_BLEND_OP SR_D3D12ConvertBlendFunc(SR_BlendFunc aBlendFunc)
{
	switch (aBlendFunc)
	{
	case SR_BlendFunc::Add:
		return D3D12_BLEND_OP_ADD;
	case SR_BlendFunc::Subtract:
		return D3D12_BLEND_OP_SUBTRACT;
	case SR_BlendFunc::ReverseSubtract:
		return D3D12_BLEND_OP_REV_SUBTRACT;
	case SR_BlendFunc::Min:
		return D3D12_BLEND_OP_MIN;
	case SR_BlendFunc::Max:
		return D3D12_BLEND_OP_MAX;
	default:
		break;
	}
	assert(false && "Unknown blend func DX12");
	return D3D12_BLEND_OP_ADD;
}

UINT8 SR_D3D12ConvertColorWriteMask(uint8 aWriteMask)
{
	UINT8 mask = 0;
	if (aWriteMask & (uint8)SR_ColorWriteMask::Red)
		mask |= D3D12_COLOR_WRITE_ENABLE_RED;
	if (aWriteMask & (uint8)SR_ColorWriteMask::Green)
		mask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
	if (aWriteMask & (uint8)SR_ColorWriteMask::Blue)
		mask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
	if (aWriteMask & (uint8)SR_ColorWriteMask::Alpha)
		mask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
	return mask;
}

SR_ShaderModel SR_D3D12ConvertShaderModel(D3D_SHADER_MODEL aShaderModel)
{
	switch (aShaderModel)
	{
	case D3D_SHADER_MODEL_6_0:
		return SR_ShaderModel::SM_6_0;
	case D3D_SHADER_MODEL_6_1:
		return SR_ShaderModel::SM_6_1;
	case D3D_SHADER_MODEL_6_2:
		return SR_ShaderModel::SM_6_2;
	case D3D_SHADER_MODEL_6_3:
		return SR_ShaderModel::SM_6_3;
	case D3D_SHADER_MODEL_6_4:
		return SR_ShaderModel::SM_6_4;
	case D3D_SHADER_MODEL_6_5:
		return SR_ShaderModel::SM_6_5;
	case D3D_SHADER_MODEL_6_6:
		return SR_ShaderModel::SM_6_6;
	case D3D_SHADER_MODEL_6_7:
		return SR_ShaderModel::SM_6_7;
	case D3D_SHADER_MODEL_5_1:
		return SR_ShaderModel::SM_5_1;
	default:
		return SR_ShaderModel::Unknown;
	}
}

D3D_SHADER_MODEL SR_D3D12ConvertShaderModel(SR_ShaderModel aShaderModel)
{
	switch (aShaderModel)
	{
	case SR_ShaderModel::SM_6_0:
		return D3D_SHADER_MODEL_6_0;
	case SR_ShaderModel::SM_6_1:
		return D3D_SHADER_MODEL_6_1;
	case SR_ShaderModel::SM_6_2:
		return D3D_SHADER_MODEL_6_2;
	case SR_ShaderModel::SM_6_3:
		return D3D_SHADER_MODEL_6_3;
	case SR_ShaderModel::SM_6_4:
		return D3D_SHADER_MODEL_6_4;
	case SR_ShaderModel::SM_6_5:
		return D3D_SHADER_MODEL_6_5;
	case SR_ShaderModel::SM_6_6:
		return D3D_SHADER_MODEL_6_6;
	case SR_ShaderModel::Unknown:
	case SR_ShaderModel::SM_5_1:
	default:
		return D3D_SHADER_MODEL_5_1;
	}
}

D3D12_FILTER SR_D3D12ConvertFilter(SR_FilterMode aMinFilter, SR_FilterMode aMagFilter, SR_FilterMode aMipFilter, SR_ComparisonFunc aCompareFunc, uint32 aAnisotropy)
{
	SR_FilterMode filterModes[3] = { aMinFilter, aMagFilter, aMipFilter };
	D3D12_FILTER_TYPE filterTypes[3];

	int reduction = -1;
	if (aCompareFunc != SR_ComparisonFunc::Never)
		reduction = D3D12_FILTER_REDUCTION_TYPE_COMPARISON;

	for (uint32 i = 0; i != 3; ++i)
	{
		const SR_FilterMode& filter = filterModes[i];

		switch (filter)
		{
		case SR_FilterMode::Point:
			filterTypes[i] = D3D12_FILTER_TYPE_POINT;
			break;
		case SR_FilterMode::Linear:
		case SR_FilterMode::Min:
		case SR_FilterMode::Max:
			filterTypes[i] = D3D12_FILTER_TYPE_LINEAR;
			break;
		default:
			assert(0);
		}

		switch (filter)
		{
		case SR_FilterMode::Point:
			break;
		case SR_FilterMode::Linear:
			assert(reduction == -1 || reduction == D3D12_FILTER_REDUCTION_TYPE_STANDARD || reduction == D3D12_FILTER_REDUCTION_TYPE_COMPARISON);
			if (reduction != D3D12_FILTER_REDUCTION_TYPE_COMPARISON)
				reduction = D3D12_FILTER_REDUCTION_TYPE_STANDARD;
			break;
		case SR_FilterMode::Min:
			assert(reduction == -1 || reduction == D3D12_FILTER_REDUCTION_TYPE_MINIMUM);
			reduction = D3D12_FILTER_REDUCTION_TYPE_MINIMUM;
			break;
		case SR_FilterMode::Max:
			assert(reduction == -1 || reduction == D3D12_FILTER_REDUCTION_TYPE_MAXIMUM);
			reduction = D3D12_FILTER_REDUCTION_TYPE_MAXIMUM;
			break;
		default:
			assert(0);
		}
	}

	if (reduction == -1)
		reduction = D3D12_FILTER_REDUCTION_TYPE_STANDARD;

	uint32 resultFilter = D3D12_ENCODE_BASIC_FILTER(filterTypes[0], filterTypes[1], filterTypes[2], reduction);

	if (aAnisotropy > 1 && (resultFilter == D3D12_FILTER_MIN_MAG_MIP_LINEAR /*|| aAnisotropy != SRSamplerProperties::gDefaultAnisotropy*/))
		resultFilter |= D3D12_ANISOTROPIC_FILTERING_BIT;

	return D3D12_FILTER(resultFilter);
}

D3D12_TEXTURE_ADDRESS_MODE SR_D3D12ConvertWrapMode(SR_WrapMode aWrapMode)
{
	switch (aWrapMode)
	{
	case SR_WrapMode::Wrap:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case SR_WrapMode::Clamp:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case SR_WrapMode::Mirror:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	case SR_WrapMode::MirrorOnce:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
	case SR_WrapMode::Border:
		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	}
	assert(false && "Unknown wrap mode");
	return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
}

D3D12_RESOURCE_DIMENSION SR_D3D12ConvertDimension(SR_ResourceType aType)
{
	switch (aType)
	{
	case SR_ResourceType::Texture1D:
		return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	case SR_ResourceType::Texture2D:
		return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	case SR_ResourceType::Texture3D:
		return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	case SR_ResourceType::Buffer:
		return D3D12_RESOURCE_DIMENSION_BUFFER;
	case SR_ResourceType::Unknown:
	default:
		return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}
}

D3D12_RESOURCE_DESC SR_GetD3D12ResourceDesc(const SR_TextureResourceProperties& aProperties)
{
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Width = static_cast<UINT64>(aProperties.mSize.x);
	resourceDesc.Height = static_cast<UINT64>(aProperties.mSize.y);

	resourceDesc.DepthOrArraySize = static_cast<UINT16>(SC_Max(aProperties.mArraySize, 1U));
	if (aProperties.mType == SR_ResourceType::Texture3D)
		resourceDesc.DepthOrArraySize = static_cast<UINT16>(aProperties.mSize.z);
	else if (aProperties.mType == SR_ResourceType::Texture2D && aProperties.mIsCubeMap)
		resourceDesc.DepthOrArraySize *= 6;

	resourceDesc.Format = SR_D3D12ConvertFormat(aProperties.mFormat);
	resourceDesc.MipLevels = aProperties.mNumMips;
	resourceDesc.Dimension = SR_D3D12ConvertDimension(aProperties.mType);
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

	if (aProperties.mAllowRenderTarget)
		resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (aProperties.mAllowDepthStencil)
		resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	if (aProperties.mAllowUnorderedAccess)
		resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	return resourceDesc;
}

D3D12_RESOURCE_DESC SR_GetD3D12ResourceDesc(const SR_BufferResourceProperties& /*aProperties*/)
{
	return D3D12_RESOURCE_DESC();
}

#endif