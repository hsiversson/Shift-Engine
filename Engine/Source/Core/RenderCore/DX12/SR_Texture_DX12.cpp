#include "SR_Texture_DX12.h"

#if ENABLE_DX12
#include "SR_TextureResource_DX12.h"
#include "SR_RenderDevice_DX12.h"
#include "SR_DescriptorHeap_DX12.h"

SR_Texture_DX12::SR_Texture_DX12(const SR_TextureProperties& aProperties, const SC_Ref<SR_TextureResource>& aTextureResource)
	: SR_Texture(aProperties, aTextureResource)
{
	mDX12Resource = static_cast<SR_TextureResource_DX12*>(GetResource());
}

SR_Texture_DX12::~SR_Texture_DX12()
{
}

bool SR_Texture_DX12::Init()
{
	return (mProperties.mWritable) ? InitAsUAV() : InitAsSRV();
}

D3D12_CPU_DESCRIPTOR_HANDLE SR_Texture_DX12::GetCPUHandle() const
{
	return D3D12_CPU_DESCRIPTOR_HANDLE{ mDescriptor.mDescriptorHandleCPU };
}

D3D12_GPU_DESCRIPTOR_HANDLE SR_Texture_DX12::GetGPUHandle() const
{
	return D3D12_GPU_DESCRIPTOR_HANDLE{ mDescriptor.mDescriptorHandleGPU };
}

bool SR_Texture_DX12::InitAsSRV()
{
	const SR_TextureResourceProperties& resourceProps = mResource->GetProperties();

	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = SR_D3D12ConvertFormat(mProperties.mFormat);
	desc.Format = SR_D3D12ConvertToValidSRVFormat(desc.Format);
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	bool isArray = resourceProps.mArraySize > 0;

	uint32 mip = mProperties.mMostDetailedMip;
	uint32 numMips = mProperties.mMipLevels ? mProperties.mMipLevels : resourceProps.mNumMips - mip;

	uint32 resourceArraySize = (resourceProps.mType == SR_ResourceType::Texture3D) ? resourceProps.mSize.z : SC_Max(resourceProps.mArraySize, 1U);
	uint32 arraySize = mProperties.mArraySize ? mProperties.mArraySize : resourceArraySize - mProperties.mFirstArrayIndex;

	switch (mProperties.mDimension)
	{
	case SR_TextureDimension::Texture1D:
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MostDetailedMip = mip;
		desc.Texture1D.MipLevels = numMips;
		break;
	case SR_TextureDimension::Texture2D:
		if (isArray)
		{
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MostDetailedMip = mip;
			desc.Texture2DArray.MipLevels = numMips;
			desc.Texture2DArray.FirstArraySlice = mProperties.mFirstArrayIndex;
			desc.Texture2DArray.ArraySize = arraySize;
		}
		else
		{
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MostDetailedMip = mip;
			desc.Texture2D.MipLevels = numMips;
		}
		break;
	case SR_TextureDimension::Texture3D:
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MostDetailedMip = mip;
		desc.Texture3D.MipLevels = numMips;
		break;
	case SR_TextureDimension::TextureCube:
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels = numMips;
		desc.TextureCube.MostDetailedMip = mip;
		break;
	default:
		SC_ASSERT(false, "Unknown dimension");
		return false;
	}

	mDescriptor = SR_RenderDevice_DX12::gInstance->GetDefaultDescriptorHeap()->Alloc();
	SR_RenderDevice_DX12::gInstance->GetD3D12Device()->CreateShaderResourceView(mDX12Resource->GetD3D12Resource(), &desc, D3D12_CPU_DESCRIPTOR_HANDLE{ mDescriptor.mDescriptorHandleCPU });

	return true;
}

bool SR_Texture_DX12::InitAsUAV()
{
	const SR_TextureResourceProperties& resourceProps = mResource->GetProperties();

	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format = SR_D3D12ConvertFormat(mProperties.mFormat);

	bool isArray = resourceProps.mArraySize > 0;

	uint32 mip = mProperties.mMostDetailedMip;
	//uint32 numMips = mProperties.mMipLevels ? mProperties.mMipLevels : resourceProps.mNumMips - mip;

	uint32 resourceArraySize = (resourceProps.mType == SR_ResourceType::Texture3D) ? resourceProps.mSize.z : SC_Max(resourceProps.mArraySize, 1U);
	uint32 arraySize = mProperties.mArraySize ? mProperties.mArraySize : resourceArraySize - mProperties.mFirstArrayIndex;

	switch (mProperties.mDimension)
	{
	case SR_TextureDimension::Texture1D:
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MipSlice = mip;
		break;
	case SR_TextureDimension::Texture2D:
		if (isArray)
		{
			desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipSlice = mip;
			desc.Texture2DArray.FirstArraySlice = mProperties.mFirstArrayIndex;
			desc.Texture2DArray.ArraySize = arraySize;
		}
		else
		{
			desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = mip;
		}
		break;
	case SR_TextureDimension::Texture3D:
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MipSlice = mip;
		desc.Texture3D.FirstWSlice = mProperties.mFirstArrayIndex;
		desc.Texture3D.WSize = arraySize;
		break;
	case SR_TextureDimension::TextureCube:
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = mip;
		desc.Texture2DArray.FirstArraySlice = mProperties.mFirstArrayIndex * 6;
		desc.Texture2DArray.ArraySize = SC_Max(arraySize, 1U) * 6;
		break;
	default:
		SC_ASSERT(false, "Unknown dimension");
		return false;
	}

	mDescriptor = SR_RenderDevice_DX12::gInstance->GetDefaultDescriptorHeap()->Alloc();
	SR_RenderDevice_DX12::gInstance->GetD3D12Device()->CreateUnorderedAccessView(mDX12Resource->GetD3D12Resource(), nullptr, &desc, D3D12_CPU_DESCRIPTOR_HANDLE{ mDescriptor.mDescriptorHandleCPU });
	return true;
}

#endif