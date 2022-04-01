#include "SR_TempResourceHeap_DX12.h"

#if ENABLE_DX12
#include "SR_RenderDevice_DX12.h"
#include "SR_Heap_DX12.h"
#include "SR_TextureResource_DX12.h"
#include "SR_BufferResource_DX12.h"

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
	heapProps.mType = SR_HeapType::Default;
	mResourceHeap_RT_DS_Textures = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_RT_DS_Textures->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}

	heapProps.mByteSize = MB(256);
	heapProps.mResourceType = SR_HeapResourceType::Texture;
	mResourceHeap_RW_Textures = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_RW_Textures->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}
	mResourceHeap_R_Textures = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_R_Textures->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}

	heapProps.mByteSize = MB(128);
	heapProps.mResourceType = SR_HeapResourceType::Buffer;
	heapProps.mType = SR_HeapType::Upload;
	mResourceHeap_ConstantBuffers = SC_MakeUnique<SR_Heap_DX12>(heapProps);
	if (!mResourceHeap_ConstantBuffers->Init())
	{
		SC_ASSERT(false, "Could not create heap.");
		return false;
	}
	return true;
}

void SR_TempResourceHeap_DX12::EndFrameInternal()
{
	mResourceHeap_RW_Textures->ResetOffset();
	mResourceHeap_ConstantBuffers->ResetOffset();
}

SR_TempTexture SR_TempResourceHeap_DX12::GetTextureInternal(const SR_TextureResourceProperties& aTextureProperties, bool aIsTexture, bool aIsRenderTarget, bool aIsWritable)
{
	SR_TextureResourceProperties resourceProps(aTextureProperties);
	resourceProps.mHeap = mResourceHeap_RW_Textures.get();
	resourceProps.mAllowRenderTarget = resourceProps.mAllowRenderTarget && aIsRenderTarget;
	resourceProps.mAllowUnorderedAccess = resourceProps.mAllowUnorderedAccess && aIsWritable;

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

SR_TempBuffer SR_TempResourceHeap_DX12::GetBufferInternal(const SR_BufferResourceProperties& aBufferProperties, bool aIsWritable)
{
	SR_BufferResourceProperties resourceProps(aBufferProperties);
	resourceProps.mHeap = mResourceHeap_ConstantBuffers.get();
	resourceProps.mWritable = resourceProps.mWritable && aIsWritable;

	SR_TempBuffer tempBuffer = {};
	SC_Ref<SR_BufferResource_DX12> dx12Resource = SC_MakeRef<SR_BufferResource_DX12>(resourceProps);
	if (dx12Resource->Init(nullptr))
	{
		tempBuffer.mResource = dx12Resource;

		//if (aIsWritable)
		//{
		//	SR_BufferProperties bufferProps;
		//	bufferProps.mElementCount = resourceProps.mElementCount;
		//	tempBuffer.mRWBuffer = SR_RenderDevice_DX12::gD3D12Instance->CreateBuffer(bufferProps, tempBuffer.mResource);
		//}
	}

	return tempBuffer;
}

#endif //ENABLE_DX12