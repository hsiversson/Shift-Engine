#include "SR_TempResourceHeap_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"
#include "SR_Heap_DX12.h"
#include "SR_TextureResource_DX12.h"

SR_TempResourceHeap_DX12::SR_TempResourceHeap_DX12()
{

}

SR_TempResourceHeap_DX12::~SR_TempResourceHeap_DX12()
{

}

bool SR_TempResourceHeap_DX12::Init()
{
	SR_HeapProperties heapProps;
	heapProps.mByteSize = GB(1);
	heapProps.mResourceType = SR_HeapResourceType::RenderTarget;
	mResourceHeap_RT_Textures = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_RT_Textures->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}

	heapProps.mByteSize = MB(64);
	heapProps.mResourceType = SR_HeapResourceType::Buffer;
	mResourceHeap_Buffers = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_Buffers->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}
	return true;
}

SR_BufferResource* SR_TempResourceHeap_DX12::GetBuffer()
{
	return nullptr;
}

SR_TempTexture SR_TempResourceHeap_DX12::GetTexture(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture, bool aIsRenderTarget, bool aIsWritable)
{
	SR_TextureResourceProperties resourceProps(aTextureProperties);
	resourceProps.mHeap = mResourceHeap_RT_Textures.get();

	SR_TempTexture tempTexture = {};
	SC_Ref<SR_TextureResource_DX12> dx12Resource = SC_MakeRef<SR_TextureResource_DX12>(resourceProps);
	if (dx12Resource->Init(nullptr, 0))
	{
		tempTexture.mResource = dx12Resource;

		if (aIsTexture)
		{
			SR_TextureProperties texProps(resourceProps.mFormat);
			tempTexture.mTexture = SR_RenderDevice_DX12::gD3D12Instance->CreateTexture(texProps, tempTexture.mResource);
		}

		if (aIsRenderTarget)
		{
			SR_RenderTargetProperties rtProps(resourceProps.mFormat);
			tempTexture.mRenderTarget = SR_RenderDevice_DX12::gD3D12Instance->CreateRenderTarget(rtProps, tempTexture.mResource);
		}

		if (aIsWritable)
		{
			SR_TextureProperties texProps(resourceProps.mFormat);
			texProps.mWritable = true;
			tempTexture.mRWTexture = SR_RenderDevice_DX12::gD3D12Instance->CreateTexture(texProps, tempTexture.mResource);
		}
	}
	
	return tempTexture;
}

#endif //ENABLE_DX12