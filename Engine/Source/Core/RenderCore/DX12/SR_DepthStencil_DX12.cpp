#include "SR_DepthStencil_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"
#include "SR_TextureResource_DX12.h"
#include "SR_DescriptorHeap_DX12.h"

SR_DepthStencil_DX12::SR_DepthStencil_DX12(const SR_DepthStencilProperties& aProperties, SC_Ref<SR_TextureResource> aResource)
	: SR_DepthStencil(aProperties, aResource)
{
	mDX12Resource = static_cast<SR_TextureResource_DX12*>(GetResource());
}

SR_DepthStencil_DX12::~SR_DepthStencil_DX12()
{
}

bool SR_DepthStencil_DX12::Init()
{
	const SR_TextureResourceProperties& textureResourceProperties = mDX12Resource->GetProperties();

	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};

	bool isArray = textureResourceProperties.mArraySize > 0;

	uint32 mip = mProperties.mMostDetailedMip;
	//uint32 numMips = mProperties.mMipLevels ? mProperties.mMipLevels : textureResourceProperties.mNumMips - mip;

	uint32 resourceArraySize = (textureResourceProperties.mType == SR_ResourceType::Texture3D) ? textureResourceProperties.mSize.z : SC_Max(textureResourceProperties.mArraySize, 1U);
	uint32 arraySize = mProperties.mArraySize ? mProperties.mArraySize : resourceArraySize - mProperties.mFirstArrayIndex;

	if (mProperties.mFormat != SR_Format::UNKNOWN)
		mProperties.mFormat = textureResourceProperties.mFormat;

	desc.Format = SR_D3D12ConvertFormat(mProperties.mFormat);

	switch (textureResourceProperties.mType)
	{
	case SR_ResourceType::Texture1D:
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
		desc.Texture1D.MipSlice = mip;
		break;
	case SR_ResourceType::Texture2D:
		if (isArray)
		{
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = arraySize;
			desc.Texture2DArray.FirstArraySlice = mProperties.mFirstArrayIndex;
			desc.Texture2DArray.MipSlice = mip;
		}
		else
		{
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = mip;
		}
		break;
	default:
		SC_ASSERT(false,"Dimension not supported.");
		break;
	}

	mDescriptor = SR_RenderDevice_DX12::gInstance->GetDSVDescriptorHeap()->Alloc();
	SR_RenderDevice_DX12::gInstance->GetD3D12Device()->CreateDepthStencilView(mDX12Resource->GetD3D12Resource(), &desc, D3D12_CPU_DESCRIPTOR_HANDLE{mDescriptor.mDescriptorHandleCPU});

	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE SR_DepthStencil_DX12::GetCPUHandle() const
{
	return D3D12_CPU_DESCRIPTOR_HANDLE{ mDescriptor.mDescriptorHandleCPU };
}

#endif