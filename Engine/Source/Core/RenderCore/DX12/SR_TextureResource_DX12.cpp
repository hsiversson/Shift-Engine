
#include "SR_TextureResource_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"
#include "SR_Heap_DX12.h"

SR_TextureResource_DX12::SR_TextureResource_DX12(const SR_TextureResourceProperties& aProperties)
	: SR_TextureResource(aProperties)
	, mD3D12Resource(nullptr)
{

}

SR_TextureResource_DX12::SR_TextureResource_DX12(const SR_TextureResourceProperties& aProperties, ID3D12Resource* aResource)
	: SR_TextureResource(aProperties)
	, mD3D12Resource(aResource)
{
	mTrackedD3D12Resource = mD3D12Resource;
}

SR_TextureResource_DX12::~SR_TextureResource_DX12()
{
	if (mD3D12Resource)
	{
		mD3D12Resource->Release();
		mD3D12Resource = nullptr;
	}
}

bool SR_TextureResource_DX12::Init(const SR_PixelData* aInitialData, uint32 aDataCount)
{
	D3D12_RESOURCE_DESC resourceDesc = SR_GetD3D12ResourceDesc(mProperties);

	bool useOptimizedClearValue = (mProperties.mAllowDepthStencil || mProperties.mAllowRenderTarget) && (resourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER);

	D3D12_CLEAR_VALUE omptimizedClearValue;
	omptimizedClearValue.Color[0] = 0.0f;
	omptimizedClearValue.Color[1] = 0.0f;
	omptimizedClearValue.Color[2] = 0.0f;
	omptimizedClearValue.Color[3] = 0.0f;
	omptimizedClearValue.Format = resourceDesc.Format;

	D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
	HRESULT hr = S_OK;

	if (mProperties.mHeap)
	{
		const D3D12_RESOURCE_ALLOCATION_INFO allocInfo = SR_RenderDevice_DX12::gD3D12Instance->GetD3D12Device()->GetResourceAllocationInfo(0, 1, &resourceDesc);

		SR_Heap_DX12* heap = static_cast<SR_Heap_DX12*>(mProperties.mHeap);

		hr = SR_RenderDevice_DX12::gD3D12Instance->GetD3D12Device()->CreatePlacedResource(
			heap->GetD3D12Heap(),
			heap->GetOffset(allocInfo.SizeInBytes, allocInfo.Alignment),
			&resourceDesc,
			initialState,
			useOptimizedClearValue ? &omptimizedClearValue : nullptr,
			IID_PPV_ARGS(&mD3D12Resource)
		);
	}
	else
	{
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		hr = SR_RenderDevice_DX12::gD3D12Instance->GetD3D12Device()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			initialState,
			useOptimizedClearValue ? &omptimizedClearValue : nullptr,
			IID_PPV_ARGS(&mD3D12Resource)
		);
	}

	if (!VerifyHRESULT(hr))
	{
		SC_ASSERT(false, "Could not create texture.");
		return false;
	}

	mTrackedD3D12Resource = mD3D12Resource;

	if (!mProperties.mDebugName.empty())
		mD3D12Resource->SetName(SC_UTF8ToUTF16(mProperties.mDebugName.c_str()).c_str());

	if (aInitialData)
		UpdatePixels(aInitialData, aDataCount);

	return true;
}

D3D12_TEXTURE_COPY_LOCATION SR_TextureResource_DX12::GetCopyLocation(uint32 aSubresourceIndex) const
{
	D3D12_TEXTURE_COPY_LOCATION location = { mD3D12Resource };
	D3D12_RESOURCE_DESC desc = SR_GetD3D12ResourceDesc(mProperties);
	location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	SR_RenderDevice_DX12::gD3D12Instance->GetD3D12Device()->GetCopyableFootprints(&desc, aSubresourceIndex, 1, 0, &location.PlacedFootprint, nullptr, nullptr, nullptr);
	return location;
}

ID3D12Resource* SR_TextureResource_DX12::GetD3D12Resource() const
{
	return mD3D12Resource;
}


void SR_MemcpyPixelData(void* aDest, const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& aDstFootprint, const SR_PixelData& aData, const SR_Format& aFormat)
{
	uint32 blockSize = SR_GetFormatBlockSize(aFormat);
	uint32 numLines = aDstFootprint.Footprint.Height / blockSize;

	uint8* dst = static_cast<uint8*>(aDest) + aDstFootprint.Offset;
	const uint8* src = static_cast<const uint8*>(aData.mData);

	for (uint32 z = 0; z < aDstFootprint.Footprint.Depth; ++z)
	{
		SC_MemcpyRect(dst, aDstFootprint.Footprint.RowPitch, src, aData.mBytesPerLine * blockSize, numLines);
		dst += aDstFootprint.Footprint.RowPitch * numLines;
		src += aData.mBytesPerSlice;
	}
}

uint32 SR_GetD3D12SubresourceIndex(const SR_TextureResourceProperties& aProperties, const SR_TextureLevel& aLevel)
{
	uint32 mipLevels = aProperties.mNumMips;
	uint32 arraySlice = aLevel.mArrayIndex;
	if (aProperties.mIsCubeMap)
		arraySlice = aLevel.mFace + aLevel.mArrayIndex * 6;

	return aLevel.mMipLevel + arraySlice * mipLevels;
}

#endif //ENABLE_DX12