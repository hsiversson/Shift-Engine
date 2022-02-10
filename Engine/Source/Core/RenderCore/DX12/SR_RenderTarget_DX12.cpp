#include "SR_RenderTarget_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"
#include "SR_TextureResource_DX12.h"
#include "SR_DescriptorHeap_DX12.h"

SR_RenderTarget_DX12::SR_RenderTarget_DX12(const SR_RenderTargetProperties& aProperties, SC_Ref<SR_TextureResource> aResource)
	: SR_RenderTarget(aProperties, aResource)
{
}

SR_RenderTarget_DX12::~SR_RenderTarget_DX12()
{
}

bool SR_RenderTarget_DX12::Init()
{
	SR_TextureResource_DX12* resource = static_cast<SR_TextureResource_DX12*>(GetResource());
	const SR_TextureResourceProperties& textureResourceProperties = resource->GetProperties();
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};

	if (mProperties.mFormat != SR_Format::UNKNOWN)
		mProperties.mFormat = textureResourceProperties.mFormat;

	desc.Format = SR_D3D12ConvertFormat(mProperties.mFormat);

	uint32 arraySize = textureResourceProperties.mArraySize;
	if (textureResourceProperties.mIsCubeMap)
		arraySize *= 6;

	bool isArray = arraySize > 1;

	switch (textureResourceProperties.mType)
	{
	case SR_ResourceType::Texture1D:
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
		break;
	case SR_ResourceType::Texture2D:
		if (isArray)
		{
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = 1;
			desc.Texture2DArray.FirstArraySlice = mProperties.mArrayIndex + mProperties.mFace;
			desc.Texture2DArray.MipSlice = mProperties.mMipLevel;
		}
		else
		{
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = mProperties.mMipLevel;
		}
		break;
	case SR_ResourceType::Texture3D:
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
		break;
	default:
		SC_ASSERT(false, "Dimension not supported.");
		break;
	}

	
	mDescriptor = SR_RenderDevice_DX12::gD3D12Instance->GetRTVDescriptorHeap()->Alloc();
	SR_RenderDevice_DX12::gD3D12Instance->GetD3D12Device()->CreateRenderTargetView(resource->GetD3D12Resource(), &desc, D3D12_CPU_DESCRIPTOR_HANDLE{ mDescriptor.mDescriptorHandleCPU });

	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE SR_RenderTarget_DX12::GetCPUHandle() const
{
	return D3D12_CPU_DESCRIPTOR_HANDLE{mDescriptor.mDescriptorHandleCPU};
}

#endif